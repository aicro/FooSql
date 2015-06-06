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

        UpdateFilter updateFilter1("update tbl_test set id = id * 10");

        map<DbLocation, DbActionFilter*> loc_filter;
        loc_filter[dbLocation2] = &updateFilter1;

        bool success = false;
        DbExecuteAction* update_action = mysqlTasks->Update(5000);

        update_action->Do(loc_filter);
        update_action->EndAction();

        DbUpdateRslt* update_rslt = (DbUpdateRslt*)update_action->GetRslt();

        vector<const DbLocation*> db_targets;
        db_targets.push_back(&dbLocation1);
        db_targets.push_back(&dbLocation2);
        db_targets.push_back(&dbLocation3);

        map<DbLocation, long long>  rslt = update_rslt->GetAffectedRows(db_targets, success);

        map<DbLocation, long long>::iterator it = rslt.begin();
        for (; it != rslt.end(); it++)
        {
            cout << "UPDATE " << it->first.ToString() << " [" << it->second << "]" << endl;
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