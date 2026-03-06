#include <casacore/tables/DataMan/Adios2StMan.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableCopy.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <boost/program_options.hpp>
#include <mpi.h>

using namespace casacore;
namespace po = boost::program_options;

int main (int argc, const char* argv[])
{
    MPI_Init(&argc, &argv);
    int comm_size, comm_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORKLD. &comm_rank);
    po::options_description hidden;
    hidden.add_options()
        ("input_ms", po::value<std::string>()->required(), "MeasurementSet to be compressed")
        ("output_ms", po::value<std::string>()->required(), "Output name of the compressed MeasurementSet")
        ("column_name", po::value<std::string>()->required(), "Name of the column to be compressed");
    po::options_description visible("Allowed Options");
    visible.add_options()
        ("help,h", "produce help message")
        ("config,c", po::value<std::string>(), "Config file name containing argument pairs like \"input_ms=test.ms\"")
        ("compressor,o", po::value<std::string>()->default_value("mgard"), "The compressor to use, see adios2 installation for valid operators (default: mgard)")
        ("error_bound,e", po::value<float>()->default_value(0.01), "Error bound used by the compressor to determine the level of compression (default: 0.01)")
        ("error_bound_type,t", po::value<std::string>()->default_value("ABS"), "The type of error bound (i.e. for MGARD, this is ABS or REL)")
        ("step_size,s", po::value<int>(), "The size of steps to split the data into (i.e. number of rows), remaining rows will be processed in the last step")
        ("num_steps,n", po::value<int>(), "The number of steps to split the data into")
        ("ADIOS2_config, a", po::value<std::string>(), "A yaml/xml config to be passed to the ADIOS2 storage manager. A useful alternative to manually setting the operator parameters.");
    
    po::positional_options_description p;
    p.add("input_ms",1);
    p.add("output_ms",1);
    p.add("column_name",1);

    po::options_description desc;
    desc.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    if (vm.count("config"))
    {  
        std::string config = vm["config"].as<std::string>();
        std::ifstream ifs(config.c_str());
        if (!ifs)
        {
            throw std::invalid_argument("Could not open config file: " + config);
        }
        else
        {
            po::store(po::parse_config_file(ifs, desc), vm);
        }
    }

    if (vm.count("help") || argc == 1)
    {
        std::cout << "Usage: compress_ms <input_ms> <output_ms> <column_name> [opts] \n" << std::endl;
        std::cout << visible << std::endl;
        return 1;
    }
    if (!vm.count("input_ms"))
    {
        throw std::invalid_argument("'input_ms' argument is required");
    }
    if (!vm.count("output_ms"))
    {
        throw std::invalid_argument("'output_ms' argument is required");
    }
    if (!vm.count("column_name"))
    {
        throw std::invalid_argument("'column_name' argument is required");
    }
    po::notify(vm);

    std::string inFile = vm["input_ms"].as<std::string>();
    std::string outFile = vm["output_ms"].as<std::string>();
    std::string colName = vm["column_name"].as<std::string>();
    std::string operation = vm["compressor"].as<std::string>();
    std::string errBound = std::to_string(vm["error_bound"].as<float>());
    std::string errBoundType = vm["error_bound_type"].as<std::string>();
    int stepsize;
    int nsteps;
    std::string configFile;

    if (vm.count("step_size") and vm.count("num_steps"))
    {
        throw std::invalid_argument("step_size and num_steps are mutually exclusive, please use one or the other");
    }
    else if (!vm.count("step_size") and !vm.count("num_steps"))
    {
        std::cout << "Warning: neither step_size nor num_steps were given, assuming a single step. This may fail for very large files." << std::endl;
        nsteps = 1;
    }
    else if (vm.count("step_size"))
    {
        stepsize = vm["step_size"].as<int>();
    }
    else
    {
        nsteps = vm["num_steps"].as<int>();
    }

    if (vm.count("ADIOS2_config"))
    {
        configFile = vm["ADIOS2_config"].as<std::string>();
    }

    {
        //define adios storage manager
        
        
        std::cout << "Opening MeasurementSet" << std::endl;

        Table msIn(inFile);
        
        TableDesc msTD(msIn.tableDesc());
        ColumnDesc msCD(msTD.columnDesc(colName));
        DataType colType = msCD.dataType();

        if (isReal(colType))
        {
            ArrayColumn<float> dataCol(msIn, colName);
            if (msCD.options() != ColumnDesc::FixedShape)
            {
                msCD.setShape(dataCol.shape(0));
            }
            
        }
        else if (isComplex(colType))
        {
            ArrayColumn<Complex> dataCol(msIn, colName);
            if (msCD.options() != ColumnDesc::FixedShape)
            {
                msCD.setShape(dataCol.shape(0));
            }
        }
        else
        {
            throw std::invalid_argument("The type of the column is unrecognised, please ensure it is either float or complex");
        }
        
        if (msCD.options() != ColumnDesc::FixedShape)
        {
            msCD.setOptions(ColumnDesc::FixedShape);
            msTD.removeColumn(colName);
            msTD.addColumn(msCD);
        }
        Table msOut;
        if (comm_rank == 0)
        {
            SetupNewTable newTab(outFile, msTD, Table::New);
            
            std::cout << "Starting Copy" << std::endl;

            //copy measurement set
            if (configFile.empty()) 
            {
                Adios2StMan adios2stman(
                    MPI_COMM_WORLD,
                    std::string(""),
                    {},
                    {{}},
                    {{{"Variable", colName},
                    {"Operator", operation},
                    {"Accuracy", errBound},
                    {"Mode", errBoundType}}});
                newTab.bindColumn(colName, adios2stman);
            }
            else
            {
                Adios2StMan::from_config_t from_config {};
                Adios2StMan adios2stman(
                    MPI_COMM_WORLD,
                    configFile,
                    from_config);
                newTab.bindColumn(colName, adios2stman);
            }
            msOut = Table(MPI_COMM_WORLD, newTab);
            ColumnDesc outColDesc = msOut.tableDesc().columnDesc(colName);
            outColDesc.setOptions(ColumnDesc::FixedShape);
            TableCopy::copySubTables(msOut, msIn);
            msOut.addRow(msIn.nrow());
            MPI_Barrier(MPI_COMM_WORLD);
        }
        else
        {
            MPI_Barrier(MPI_COMM_WORLD);
            msOut = Table(MPI_COMM_WORLD, outFile);
        }
        int nrows, laststepsize;
        IPosition cellShape;
        Slicer rwslice;
        
        for (uInt i=0; i<msTD.ncolumn(); i++)
        {
            std::string colName_i = msTD.columnDesc(i).name();
            // Each rank takes a column as long as it's not the column to be compressed.
            if (comm_rank % msTD.ncolumn() == i % comm_size)
            {
                std::cout << "Copying Column: " + colName_i << std::endl;
            }
            if (colName_i == colName)
            {
                //Stop all other ranks here to do this copy together
                MPI_Barrier(MPI_COMM_WORLD);
                if (isReal(colType))
                {
                    ArrayColumn<float> dataCol(msIn, colName);
                    nrows = dataCol.nrow();
                    if (stepsize)
                    {
                        nsteps = nrows/stepsize;
                        laststepsize = nrows - nsteps*stepsize;
                    }
                    else
                    {
                        stepsize = nrows/nsteps;
                        laststepsize = nrows - nsteps*stepsize;
                    }
                    cellShape = dataCol.shape(0);
                    Array<float> data(cellShape.concatenate(IPosition(1,stepsize)));
                    ArrayColumn<float> outCol(msOut, colName);
                    if (comm_rank < nsteps)
                    {
                        for (int i = 0; i < nsteps; i++)
                        {
                            if (comm_rank % nsteps == i % comm_size)
                            {
                                rwslice = Slicer(IPosition(1,i*stepsize), IPosition(1, stepsize));
                                dataCol.getColumnRange(rwslice, data, True);
                                std::cout << "Operating on step " + std::to_string(i) + " with shape " + data.shape().toString() << std::endl;
                                outCol.putColumnRange(rwslice, data);
                            }
                        }
                    }
                    if (laststepsize > 0 && comm_rank == 0)
                    {
                        rwslice = Slicer(IPosition(1,nsteps*stepsize), IPosition(1, laststepsize));
                        dataCol.getColumnRange(rwslice, data, True);
                        std::cout << "Adding Last step with shape " + data.shape().toString() << std::endl;
                        outCol.putColumnRange(rwslice, data);
                    }
                }
                else if (isComplex(colType))
                {
                    ArrayColumn<Complex> dataCol(msIn, colName);
                    nrows = dataCol.nrow();
                    if (stepsize)
                    {
                        nsteps = nrows/stepsize;
                        laststepsize = nrows - nsteps*stepsize;
                    }
                    else
                    {
                        stepsize = nrows/nsteps;
                        laststepsize = nrows - nsteps*stepsize;
                    }
                    cellShape = dataCol.shape(0);
                    Array<Complex> data(cellShape.concatenate(IPosition(1,stepsize)));
                    ArrayColumn<Complex> outCol(msOut, colName);
                    if (comm_rank < nsteps)
                    {
                        for (int i = 0; i < nsteps; i++)
                        {
                            if (comm_rank % nsteps == i % comm_size)
                            {
                                rwslice = Slicer(IPosition(1,i*stepsize), IPosition(1, stepsize));
                                dataCol.getColumnRange(rwslice, data, True);
                                std::cout << "Operating on step " + std::to_string(i) + " with shape " + data.shape().toString() << std::endl;
                                outCol.putColumnRange(rwslice, data);
                            }
                        }
                    }
                    if (laststepsize > 0 && comm_rank == 0)
                    {
                        rwslice = Slicer(IPosition(1,nsteps*stepsize), IPosition(1, laststepsize));
                        dataCol.getColumnRange(rwslice, data, True);
                        std::cout << "Adding Last step with shape " + data.shape().toString() << std::endl;
                        outCol.putColumnRange(rwslice, data);
                    }
                }
            }
            else
            {
                TableCopy::copyColumnData(msIn, colName_i, msOut, colName_i, false);
            }
        }   
    }

    //check output
    MPI_Finalize();
    std::cout << "Done" << std::endl;
    return 0;
}