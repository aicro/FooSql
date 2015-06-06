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

        QueryFilter selectFilter1("select id, name, uptime from tbl_test limit 2");
        QueryFilter selectFilter3("select id, name, uptime from tbl_test limit 2");

        map<DbLocation, DbActionFilter*> loc_filter;
        loc_filter[dbLocation1] = &selectFilter1;
        loc_filter[dbLocation3] = &selectFilter3;

        bool success = false;
        DbQueryAction* query_action = mysqlTasks->Select();
        query_action->Do(loc_filter);

        DbQueryRslt* query_rslt = (DbQueryRslt*)query_action->GetRslt();

        // Get rows from different connections
        vector<DbLocation*> db_targets;
        db_targets.push_back(&dbLocation1);
        db_targets.push_back(&dbLocation3);

        map<DbLocation, Row> rslt;
        int count = 0;

        while(1)
        {
            rslt = query_rslt->Fetch(db_targets, success);
            map<DbLocation, unsigned long*> lengths = query_rslt->GetCurrentRowColumnsLength(db_targets, success);

            if (rslt.size() == 0)
            {
				//  all rows have been fetched
                break;
            }

            for (int i = 0; i < db_targets.size(); i++)
            {
                if (rslt.find(*(db_targets[i])) == rslt.end())
                {
                    cout << "No data from" << db_targets[i]->ToString() << endl;
                    continue;
                }
                else
                {
                    cout << "Data from " << db_targets[i]->ToString() << endl;
                }

                char** row = rslt[*(db_targets[i])];
                unsigned long* l = lengths[*(db_targets[i])];

                if (row != NULL)
                {
                    cout << "id   = " << row[0] << "(" << l[0] << ")" << endl;
                    cout << "name = " << row[1] << "(" << l[1] << ")" << endl;
                    cout << "time = " << row[2] << "(" << l[2] << ")" << endl << endl;
                }
            }
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