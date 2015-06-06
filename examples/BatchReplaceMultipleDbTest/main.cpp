#include <vector>
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

        DbExecuteAction* insert_action = mysqlTasks->BatchReplace(5000, 2);

        int row_count = 0;
        while (row_count++ < 3)
        {
            map<string, Value> column_value;

            column_value["id"] = Value((long long)(row_count));
            column_value["name"] = Value((long long)(7), "ab\0defg");
            column_value["uptime"] = Value("NOW()", true);

			BatchFilter filter("tbl_test", column_value);
			
            map<DbLocation, DbActionFilter*> loc_filter;
            loc_filter[dbLocation2] = &filter;
            loc_filter[dbLocation3] = &filter;
            
            insert_action->Do(loc_filter);
        }
        insert_action->EndAction();
		
        DbInsertRslt* insert_rslt = (DbInsertRslt*)insert_action->GetRslt();

        bool success = true;
        map<DbLocation, long long> affected_rows = insert_rslt->GetAffectedRows(success);
        map<DbLocation, long long>::iterator it = affected_rows.begin();
        for (; it != affected_rows.end(); it++)
        {
            cout << "INSERT INTO " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

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