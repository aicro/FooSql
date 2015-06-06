#include <iostream>
#include <tr1/memory>

#include "dbcomm/DbComm.h"
#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::DBCOMM;
using namespace COMMON::EXCEPTION;

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

        DbQueryAction* query_action = mysqlTasks->Select();

        QueryFilter selectFilter("select id, name   , uptime from tbl_test");
        query_action->Do(&selectFilter);

        DbQueryRslt* query_rslt = (DbQueryRslt*)query_action->GetRslt();

        Row rslt;
        int count = 0;
        bool success = false;

        while ((char**)(rslt = query_rslt->Fetch(success)) != NULL) 
        {
            unsigned long * lengths = query_rslt->GetCurrentRowColumnsLength(success);

            cout << "id   = " << rslt[0] << "(" << lengths[0] << ")" << endl;
            cout << "name = " << rslt[1] << "(" << lengths[1] << ")" << endl;
            cout << "time = " << rslt[2] << "(" << lengths[2] << ")" << endl << endl;
        }

        query_action->EndAction();
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