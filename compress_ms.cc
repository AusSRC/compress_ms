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
    if ( argc != 5 ) 
    {
        std::cout << "Usage: compress_ms <input_ms> <output_ms> <column_name> <error_bound> <ABS/REL>" << std:: endl;
        return -1;
    }
    else
    {
        inFile = argv[0];
        outFile = argv[1];
        colName = argv[2];
        errBound = argv[3];
        errBoundType = argv[4]; 
    }

    {
        //define adios storage manager
        Adios2StMan adios2stman(std::string(""),{},{{}},{{{"Variable", colName},{"Operator", "mgard"},{"Accuracy", errBound}}});
        
        MeasurementSet msIn(inFile);
        ArrayColumn<Complex> dataCol(msIn, colName);
        TableDesc td("", "1", TableDesc::Scratch);
        td.addColumn(ArrayColumnDesc<Complex>(colName, dataCol.shape(0), ColumnDesc::FixedShape));
        SetupNewTable newTab(outFile, td, Table::New);
        
        //copy measurement set
        newTab.bindAll(adios2stman);
        MeasurementSet msOut(newTab);
        msOut.addRow(msIn.nrow());
        TableCopy::copySubTables(msOut, msIn);
        TableCopy::copyColumnData(msIn, colName, msOut, colName, false);
        

        
        
    }

    //check output
    // MPI_Finalize();
    return 0;
}