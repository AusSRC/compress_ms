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
    //parse args
        //original filepath
        //output filepath
        //column name
        //compression error bound
        //ABS/REL error bound
    std::string inFile, outFile, operation, colName, errBound, errBoundType, configFile;
    if ( argc == 7 ) 
    {
        std::cout << "Assuming manual parameter entries" << std::endl;
        inFile = argv[1];
        outFile = argv[2];
        operation = argv[3];
        colName = argv[4];
        errBound = argv[5];
        errBoundType = argv[6]; 

        std::cout << "Parameters received: \n \
        \tinput_ms: " + inFile + "\n \
        \toutput_ms: " + outFile + "\n \
        \toperator: " + operation + "\n \
        \tcolumn_name: " + colName + "\n \
        \terror_bound: " + errBound + "\n \
        \tABS/REL: " + errBoundType << std::endl;

        if ( errBoundType != "ABS" && errBoundType != "REL")
        {
            throw std::invalid_argument("Ensure that the <ABS/REL> parameter is either ABS or REL");
        }
        try 
        {
            std::cout << "The error bound parsed as a number is " + std::to_string(std::stof(errBound)) << std::endl;
        }
        catch (const std::invalid_argument& ex)
        {
            throw std::invalid_argument("Error bound must be a number.");
        }
    }
    else if ( argc == 5)
    {
        std::cout << "Assuming config file entry" << std::endl;
        inFile = argv[1];
        outFile = argv[2];
        colName = argv[3];
        configFile = argv[4];

        std::cout << "Parameters received: \n \
        \tinput_ms: " + inFile + "\n \
        \toutput_ms: " + outFile + "\n \
        \tcolumn_name: " + colName + "\n \
        \tconfig_file: " + configFile << std::endl;
    }
    else
    {
        std::cout << "Usage: compress_ms <input_ms> <output_ms> <operator> <column_name> <error_bound> <ABS/REL>\n \
        \tcompress_ms <input_ms> <output_ms> <column_name> <config_file>" << std:: endl;
        return -1;
    }
    
    {
        //define adios storage manager
        
        
        std::cout << "Opening MeasurementSet" << std::endl;

        MeasurementSet msIn(inFile);
        
        TableDesc msTD(msIn.tableDesc());
        ColumnDesc msCD(msTD.columnDesc(colName));
        DataType colType = msCD.dataType();
        if (isReal(colType))
        {
            ArrayColumn<float> dataCol(msIn, colName);
            msCD.setShape(dataCol.shape(0));
        }
        else if (isComplex(colType))
        {
            ArrayColumn<Complex> dataCol(msIn, colName);
            msCD.setShape(dataCol.shape(0));
        }
        else
        {
            throw std::invalid_argument("The type of the column is unrecognised, please ensure it is either float or complex");
        }
        
        msCD.setOptions(ColumnDesc::FixedShape);
        msTD.removeColumn(colName);
        msTD.addColumn(msCD);
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
        MeasurementSet msOut(newTab);
        ColumnDesc outColDesc = msOut.tableDesc().columnDesc("DATA");
        outColDesc.setOptions(ColumnDesc::FixedShape);
        TableCopy::copySubTables(msOut, msIn);
        msOut.addRow(msIn.nrow());
        
        for (uInt i=0; i<msTD.ncolumn(); i++)
        {
            std::string colName_i = msTD.columnDesc(i).name();
            std::cout << "Copying Column: " + colName_i << std::endl;
            if (colName_i == colName)
            {
                if (isReal(colType))
                {
                    ArrayColumn<float> dataCol(msIn, colName);
                    Array<float> data = dataCol.getColumn();
                    std::cout << data.shape() << std::endl;
                    ArrayColumn<float> outCol(msOut, colName);
                    outCol.putColumn(data);
                }
                else if (isComplex(colType))
                {
                    ArrayColumn<Complex> dataCol(msIn, colName);
                    Array<Complex> data = dataCol.getColumn();
                    std::cout << data.shape() << std::endl;
                    ArrayColumn<Complex> outCol(msOut, colName);
                    outCol.putColumn(data);
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