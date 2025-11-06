#include <casacore/tables/DataMan/Adios2StMan.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableCopy.h>
#include <casacore/tables/Tables/SetupNewTab.h>
// #include <mpi.h>

using namespace casacore;

int main (int argc, const char* argv[])
{
    // MPI_Init(&argc, &argv);
    std::string inFile, outFile, operation, colName, errBound, errBoundType, configFile, stepsize_str;
    int stepsize;
    if ( argc == 8 ) 
    {
        std::cout << "Assuming manual parameter entries" << std::endl;
        inFile = argv[1];
        outFile = argv[2];
        operation = argv[3];
        colName = argv[4];
        errBound = argv[5];
        errBoundType = argv[6];
        stepsize_str = argv[7];


        std::cout << "Parameters received: \n \
        \tinput_ms: " + inFile + "\n \
        \toutput_ms: " + outFile + "\n \
        \toperator: " + operation + "\n \
        \tcolumn_name: " + colName + "\n \
        \tABS/REL: " + errBoundType << std::endl;

        if ( errBoundType != "ABS" && errBoundType != "REL")
        {
            throw std::invalid_argument("Ensure that the <ABS/REL> parameter is either ABS or REL");
        }
        try 
        {
            std::cout << "Error Bound: " + std::to_string(std::stof(errBound)) << std::endl;
        }
        catch (const std::invalid_argument& ex)
        {
            throw std::invalid_argument("Error bound must be a number.");
        }
    }
    else if ( argc == 6)
    {
        std::cout << "Assuming config file entry" << std::endl;
        inFile = argv[1];
        outFile = argv[2];
        colName = argv[3];
        configFile = argv[4];
        stepsize_str = argv[5];

        std::cout << "Parameters received: \n \
        \tinput_ms: " + inFile + "\n \
        \toutput_ms: " + outFile + "\n \
        \tcolumn_name: " + colName + "\n \
        \tconfig_file: " + configFile << std::endl;
    }
    else
    {
        std::cout << "Usage: compress_ms <input_ms> <output_ms> <operator> <column_name> <error_bound> <ABS/REL> <stepsize>\n \
        \tcompress_ms <input_ms> <output_ms> <column_name> <config_file> <stepsize>" << std:: endl;
        return -1;
    }
    
    try
    {
        stepsize = std::stoi(stepsize_str);
        std::cout << "Step Size: " + std::to_string(stepsize) << std::endl;
    }
    catch (const std::invalid_argument& ex)
    {
        throw std::invalid_argument("Step size must be an integer.");
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
        
        SetupNewTable newTab(outFile, msTD, Table::New);
        
        std::cout << "Starting Copy" << std::endl;

        //copy measurement set
        if (configFile.empty()) 
        {
            Adios2StMan adios2stman(
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
                configFile,
                from_config);
            newTab.bindColumn(colName, adios2stman);
        }
        Table msOut(newTab);
        ColumnDesc outColDesc = msOut.tableDesc().columnDesc(colName);
        outColDesc.setOptions(ColumnDesc::FixedShape);
        TableCopy::copySubTables(msOut, msIn);
        msOut.addRow(msIn.nrow());

        int nrows, nsteps, laststepsize;
        IPosition cellShape;
        Slicer rwslice;
        
        for (uInt i=0; i<msTD.ncolumn(); i++)
        {
            std::string colName_i = msTD.columnDesc(i).name();
            std::cout << "Copying Column: " + colName_i << std::endl;
            if (colName_i == colName)
            {
                if (isReal(colType))
                {
                    ArrayColumn<float> dataCol(msIn, colName);
                    nrows = dataCol.nrow();
                    nsteps = nrows/stepsize;
                    laststepsize = nrows - nsteps*stepsize;
                    cellShape = dataCol.shape(0);
                    Array<float> data(cellShape.concatenate(IPosition(1,stepsize)));
                    ArrayColumn<float> outCol(msOut, colName);
                    for (int i = 0; i < nsteps; i++)
                    {
                        rwslice = Slicer(IPosition(1,i*stepsize), IPosition(1, stepsize));
                        dataCol.getColumnRange(rwslice, data, True);
                        std::cout << "Operating on step " + std::to_string(i) + " with shape " + data.shape().toString() << std::endl;
                        outCol.putColumnRange(rwslice, data);
                    }
                    if (laststepsize > 0)
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
                    nsteps = nrows/stepsize;
                    laststepsize = nrows - nsteps*stepsize;
                    cellShape = dataCol.shape(0);
                    Array<Complex> data(cellShape.concatenate(IPosition(1,stepsize)));
                    ArrayColumn<Complex> outCol(msOut, colName);
                    for (int i = 0; i < nsteps; i++)
                    {
                        rwslice = Slicer(IPosition(1,i*stepsize), IPosition(1, stepsize));
                        dataCol.getColumnRange(rwslice, data, True);
                        std::cout << "Operating on step " + std::to_string(i) + " with shape " + data.shape().toString() << std::endl;
                        outCol.putColumnRange(rwslice, data);
                    }
                    if (laststepsize > 0)
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
    // MPI_Finalize();
    std::cout << "Done" << std::endl;
    return 0;
}