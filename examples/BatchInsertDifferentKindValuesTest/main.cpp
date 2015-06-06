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

        DbExecuteAction* insert_action = mysqlTasks->BatchInsert(5, 2);

        // insert into t(id, name) values(10000, 'abc')
        {
            // An better way is to use the BatchFilter::AppendCOlumnValue()
            map<string, Value> column_value;
            
            column_value["id"] = Value((long long)(10000));
            column_value["name"] = Value("abc");

			// force to check the compatibility
            BatchFilter insertFilter("tbl_test", column_value, true);
            insert_action->Do(&insertFilter);
        }
        
        // insert into t(id, uptime) values(20000, 'NOW()')  
        {
			// An better way is to use the BatchFilter::AppendCOlumnValue()
            map<string, Value> column_value;
            
            column_value["id"] = Value((long long)(20000));
            column_value["uptime"] = Value("NOW()", true, false);

            BatchFilter insertFilter("tbl_test", column_value, true);
            insert_action->Do(&insertFilter);
        }
        
        // insert into t(id, name) values(30001, 'abc')
        {
			// An better way is to use the BatchFilter::AppendCOlumnValue()
            map<string, Value> column_value;
            
            column_value["id"] = Value((long long)(30001));
            column_value["name"] = Value("abc");

            BatchFilter insertFilter("tbl_test", column_value, true);
            insert_action->Do(&insertFilter);
        }
        
        // insert into t(id, name) values(30002, 'abc')
        {
			// An better way is to use the BatchFilter::AppendCOlumnValue()
            map<string, Value> column_value;
            
            column_value["id"] = Value((long long)(30002));
            column_value["name"] = Value("abc");

            BatchFilter insertFilter("tbl_test", column_value, true);
            insert_action->Do(&insertFilter);
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