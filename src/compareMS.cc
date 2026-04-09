#include <casacore/tables/DataMan/Adios2StMan.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableCopy.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <boost/program_options.hpp>
#include <adios2.h>
#include <mpi.h>

using namespace casacore;
template <class T>
void comparecolname(Table &oldTable, Table &newTable,const std::string &colName){
    int nrows,newnrows;
    IPosition cellShape;
    ArrayColumn<T> oldcol(oldTable,colName);
    ArrayColumn<T> newcol(newTable,colName);
    cellShape = oldcol.shape(0);
    nrows=oldcol.nrow();
    newnrows=newcol.nrow();
    Array<T> olddata(cellShape.concatenate(IPosition(1,nrows)));
    Array<T> newdata(cellShape.concatenate(IPosition(1,nrows)));
    std::cout<<"nrow="<<nrows<<","<<newnrows<<std::endl;
    double total_error=0;
    /*Now we Get the datw into the arrays*/
    oldcol.getColumn(olddata);
    newcol.getColumn(newdata);

    for (uint i=0; i<cellShape[0]; ++i){
        for (uint j=0; j<cellShape[1];++j){
            for (uint k=0; k<nrows; ++k){
                std::cout<<olddata(IPosition(3,i,j,k))<<","<<newdata(IPosition(3,i,j,k))<<std::endl;
                total_error+=std::sqrt(std::pow(newdata(IPosition(3,i,j,k))-olddata(IPosition(3,i,j,k)),2));      
            }
        }
    }
    std::cout<<cellShape[0]*cellShape[1]*nrows<<std::endl;
   total_error=total_error/(cellShape[0]*cellShape[1]*nrows);
   std::cout<<"mean error="<<total_error<<std::endl;
}

int main(int argc, char **argv){
    /* This program will take two strings argument hoping to get the respective column and check the overall error*/
    const std::string oldMS=argv[1];
    const std::string newMS=argv[2];
    const std::string colName=argv[3];
    Table oldTable(oldMS,Table::Old);
    Table newTable(newMS,Table::Old);

    comparecolname<Float>(oldTable, newTable, colName);
    

    return 0;
}