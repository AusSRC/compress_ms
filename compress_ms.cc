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
    std::string inFile, outFile, colName, errBound, errBoundType;
    if ( argc != 6 ) 
    {
        std::cout << "Usage: compress_ms <input_ms> <output_ms> <column_name> <error_bound> <ABS/REL>" << std:: endl;
        return -1;
    }
    else
    {
        inFile = argv[1];
        outFile = argv[2];
        colName = argv[3];
        errBound = argv[4];
        errBoundType = argv[5]; 
    }
        std::cout << "Parameters received: \n \
            \tinput_ms: " + inFile + "\n \
            \toutput_ms: " + outFile + "\n \
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
    {
        //define adios storage manager
        Adios2StMan adios2stman(
            std::string(""),
            {},
            {{}},
            {{{"Variable", colName},
            {"Operator", "mgard"},
            {"Accuracy", errBound},
            {"Mode", errBoundType}}});
        
        std::cout << "Opening MeasurementSet" << std::endl;

        MeasurementSet msIn(inFile);
        ArrayColumn<Complex> dataCol(msIn, colName);
        TableDesc msTD = msIn.tableDesc();
        SetupNewTable newTab(outFile, msTD, Table::New);
        
        std::cout << "Starting Copy" << std::endl;
        //copy measurement set
        newTab.bindColumn(colName, adios2stman);
        MeasurementSet msOut(newTab);
        TableCopy::copySubTables(msOut, msIn);
        msOut.addRow(msIn.nrow());
        
        for (uInt i=0; i<msTD.ncolumn(); i++)
        {
            std::string colName_i = msTD.columnDesc(i).name();
            std::cout << "Copying Column: " + colName_i << std::endl;
            TableCopy::copyColumnData(msIn, colName_i, msOut, colName_i, false);
        }   
    }

    //check output
    // MPI_Finalize();
    std::cout << "Done" << std::endl;
    return 0;
}