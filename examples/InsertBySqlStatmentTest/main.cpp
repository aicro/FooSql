#include <vector>
#include <iostream>
#include <tr1/memory>

#include "dbcomm/DbComm.h"
#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::DBCOMM;
using namespace COMMON::EXCEPTION;

// To do the following 3 statements
//DB1
//insert into tbl_test values (1, '1', '20140101'), (2, '20', '20140105'), (3, '300', '20140105');

//DB2
//insert into tbl_test values (4, '4', '20140101'), (5, '50', '20140105'), (6, '600', '20140105');

//DB3
//insert into tbl_test values (7, '7', '20140101'), (8, '80', '20140105'), (9, '900', '20140105');

int main()
{
	DbLocation dbLocation1;
	dbLocation1.SetDbId("TEST_DB1");
    dbLocation1.SetIp("127.0.0.1");
    dbLocation1.SetPort("3306");
    dbLocation1.SetUser("root");
    dbLocation1.SetPassword("123456");

	DbLocation dbLocation2;
    dbLocation2.SetDbId("TEST_DB2");
    dbLocation2.SetIp("127.0.0.1");
    dbLocation2.SetPort("3306");
    dbLocation2.SetUser("root");
    dbLocation2.SetPassword("123456");
    
	DbLocation dbLocation3;
    dbLocation3.SetDbId("TEST_DB3");
    dbLocation3.SetIp("127.0.0.1");
    dbLocation3.SetPort("3306");
    dbLocation3.SetUser("root");
    dbLocation3.SetPassword("123456");
	
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);
        dbLocations_array.push_back(dbLocation2);
        dbLocations_array.push_back(dbLocation3);

        tr1::shared_ptr<IDbTasks> mysqlTasks( new MysqlDbTasks(dbLocations_array, true) );
        mysqlTasks->Connect();

        bool success = false;

        DbExecuteAction* action = mysqlTasks->Insert(5000);

        InsertFilter filter1("insert into tbl_test values (1, '1', '20140101'), (2, '20', '20140105'), (3, '300', '20140105')");
        InsertFilter filter2("insert into tbl_test values (4, '4', '20140101'), (5, '50', '20140105'), (6, '600', '20140105')");
        InsertFilter filter3("insert into tbl_test values (7, '7', '20140101'), (8, '80', '20140105'), (9, '900', '20140105')");

        map<DbLocation, DbActionFilter*> loc_filter;
        loc_filter[dbLocation1] = &filter1;
        loc_filter[dbLocation2] = &filter2;
        loc_filter[dbLocation3] = &filter3;

        action->Do(loc_filter);
        action->EndAction();

        mysqlTasks->Disconnect();
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
    catch (...)
    {
        cout << "unknown exception" << std::endl;
    }
	
	return 0;
}