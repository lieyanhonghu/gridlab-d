/*
 * helics_msg.h
 *
 *  Created on: Mar 15, 2017
 *      Author: afisher
 */

#ifndef CONNECTION_HELICS_MSG_H_
#define CONNECTION_HELICS_MSG_H_

#include "gridlabd.h"
#include "varmap.h"
#include "connection.h"
#if HAVE_HELICS
#include <helics/application_api/application_api.h>
#endif
#include<sstream>
#include<vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <json/json.h>
using std::string;
using std::vector;

class helics_msg;

///< Function relays
typedef struct s_functionsrelay {
	char localclass[64]; ///< local class info
	char localcall[64]; ///< local function call address (NULL for outgoing)
	char remoteclass[64]; ///< remote class name
	char remotename[64]; ///< remote function name
	helics_msg *route; ///< routing of relay
	TRANSLATOR *xlate; ///< output translation call
	struct s_functionsrelay *next;
	DATAEXCHANGEDIRECTION drtn;
	COMMUNICATIONTYPE ctype; ///<identifies which helics communication function to call. Only used for communicating with helics.
} FUNCTIONSRELAY;
extern "C" FUNCTIONADDR add_helics_function(helics_msg *route, const char *fclass,const char *flocal, const char *rclass, const char *rname, TRANSLATOR *xlate, DATAEXCHANGEDIRECTION direction, COMMUNICATIONTYPE ctype );
extern "C" FUNCTIONSRELAY *find_helics_function(const char *rclass, const char *rname);
extern "C" size_t helics_from_hex(void *buf, size_t len, const char *hex, size_t hexlen);

typedef enum {
	FT_VOID,
	FT_LIST,
	FT_REAL,
	FT_INTEGER,
	FT_STRING,
} HELICSTYPE;

typedef struct _helicslist {
	HELICSTYPE type;
	char tag[32];
	union {
		double real;
		int64 integer;
		struct _helicslist *list;
	};
	char st[1024];
	struct _helicslist *parent; // parent list (NULL for head)
	struct _helicslist *next;
} HELICSLIST;
class helics_publication {
	helics_publication(){
		pObjectProperty = NULL;
		pHelicsPublicationId = NULL;
	}
public:
	string objectName;
	string propertyName;
	gld_property *pObjectProperty;
	helics::pulication_id_t *pHelicsPublicationId;
};

class helics_subscription {
	helics_subscription(){
		pObjectProperty = NULL;
		pHelicsSubscriptionId = NULL;
	}
public:
	string objectName;
	string propertyName;
	string subscription_topic;
	gld_property *pObjectProperty;
	helics::subscription_id_t *pHelicsSubscriptionId;
};
class helics_msg : public gld_object {
public:
	GL_ATOMIC(double,version);
	string *port;
	string *header_version;
	string *hostname;
	char1024 configFile;
	// TODO add published properties here

private:
	vector<helics_subscription*> helics_subscriptions;
	vector<helics_publication*> helics_publications;
	vector<string> *inFunctionTopics;
	varmap *vmap[14];
	helics::ValueFederate *helics_federate;
	TIMESTAMP last_approved_helics_time;
	TIMESTAMP initial_sim_time;
	double last_delta_helics_time;
	bool exitDeltamode;
	// TODO add other properties here as needed.

public:
	// required implementations
	helics_msg(MODULE*);
	int create(void);
	int init(OBJECT* parent);
	int precommit(TIMESTAMP t1);
	TIMESTAMP presync(TIMESTAMP t1);
	TIMESTAMP sync(TIMESTAMP t1);
	TIMESTAMP postsync(TIMESTAMP t1);
	TIMESTAMP commit(TIMESTAMP t0,TIMESTAMP t1);
	int prenotify(PROPERTY* p,char* v);
	int postnotify(PROPERTY* p,char* v);
	int finalize(void);
	TIMESTAMP plc(TIMESTAMP t1);
	int route(char *value);
	int option(char *value);
	int publish(char *value);
	int subscribe(char *value);
	int configure(char *value);
	int parse_helics_function(char *value, COMMUNICATIONTYPE comstype);
	void incoming_helics_function(void);
	int publishVariables(varmap *wmap);
	int subscribeVariables(varmap *rmap);
	int publishJsonVariables( );   //Renke add
	int subscribeJsonVariables( );  //Renke add
	int publish_helicsjson_link();  //Renke add
	char simulationName[1024];
	void term(TIMESTAMP t1);
	int helics_link(char *value, COMMUNICATIONTYPE comtype);
	TIMESTAMP clk_update(TIMESTAMP t1);
	int get_varmapindex(const char *);
	SIMULATIONMODE deltaInterUpdate(unsigned int delta_iteration_counter, TIMESTAMP t0, unsigned int64 dt);
	SIMULATIONMODE deltaClockUpdate(double t1, unsigned long timestep, SIMULATIONMODE sysmode);
	// TODO add other event handlers here

public:
	static HELICSLIST *parse(char *buffer);
	static HELICSLIST *find(HELICSLIST *list, const char *tag);
	static char *get(HELICSLIST *list, const char *tag);
	static void destroy(HELICSLIST *list);
	Json::Value publish_json_config;  //add by Renke
	Json::Value publish_json_data;    //add by Renke
	Json::Value subscribe_json_data;  //add by Renke
	string publish_json_key; //add by Renke
	string subscribe_json_key; //add by Renke
	vector <string> vjson_publish_gld_property_name;
	vector <gld_property*> vjson_publish_gld_property;
public:
	// special variables for GridLAB-D classes
	static CLASS *oclass;
	static helics_msg *defaults;
};

#endif /* CONNECTION_HELICS_MSG_H_ */
