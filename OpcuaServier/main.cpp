#include <signal.h>
#include <stdio.h>
#include "open62541.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <windows.h>
#include <thread>
using namespace std;
using namespace cv;
static UA_NodeId pointTypeId;
Mat img;
cv::VideoCapture cap(0);
cv::Mat frame;
#define frame_pixel 307200*3
static void
addVariableTypeframe(UA_Server *server) {
	UA_VariableTypeAttributes vtAttr = UA_VariableTypeAttributes_default;
	vtAttr.dataType = UA_TYPES[UA_TYPES_BYTE].typeId;
	vtAttr.valueRank = 1; /* array with one dimension */
	UA_UInt32 arrayDims[1] = { frame_pixel };
	vtAttr.arrayDimensions = arrayDims;
	vtAttr.arrayDimensionsSize = 1;
	vtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "frame Type");

	/* a matching default value is required */
	UA_Byte zero[frame_pixel] = { 14, 30,42 };
	
	UA_Variant_setArray(&vtAttr.value, zero, frame_pixel, &UA_TYPES[UA_TYPES_BYTE]);

	UA_Server_addVariableTypeNode(server, UA_NODEID_NULL,
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
		UA_QUALIFIEDNAME(1, "frame Type"), UA_NODEID_NULL,
		vtAttr, NULL, &pointTypeId);
}
UA_Int32 myInteger = 42;
UA_Int32 gonum = 90;
static void
addVariable(UA_Server *server) {
	/* Define the attribute of the myInteger variable node */
	///////////////////////////////////////////////////////////
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	
	UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
	attr.description = UA_LOCALIZEDTEXT("en-US", "the answer");
	attr.displayName = UA_LOCALIZEDTEXT("en-US", "the answer");
	attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

	/* Add the variable node to the information model */
	UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
	UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
	UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
	UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
	UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
		parentReferenceNodeId, myIntegerName,
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
////////////////////////////////////////////////////////////////////////////////////////////
	UA_VariableAttributes qttr = UA_VariableAttributes_default;

	UA_Variant_setScalar(&qttr.value, &gonum, &UA_TYPES[UA_TYPES_INT32]);
	qttr.description = UA_LOCALIZEDTEXT("en-US", "go");
	qttr.displayName = UA_LOCALIZEDTEXT("en-US", "go");
	qttr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	qttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

	/* Add the variable node to the information model */
	UA_NodeId goId = UA_NODEID_STRING(1, "go");
	UA_QualifiedName goName = UA_QUALIFIEDNAME(1, "go");
	UA_Server_addVariableNode(server, goId, parentNodeId,
		parentReferenceNodeId, goName,
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), qttr, NULL, NULL);
/////////////////////////////////////////////////////////////////////////
	UA_VariableAttributes vAttr = UA_VariableAttributes_default;
	vAttr.dataType = UA_TYPES[UA_TYPES_BYTE].typeId;
	vAttr.valueRank = 1; /* array with one dimension */
	UA_UInt32 arrayDims[1] = { frame_pixel };
	vAttr.arrayDimensions = arrayDims;
	vAttr.arrayDimensionsSize = 1;
	vAttr.description = UA_LOCALIZEDTEXT("en-US", "frame Variable");
	vAttr.displayName = UA_LOCALIZEDTEXT("en-US", "frame Variable");
	vAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	/* vAttr.value is left empty, the server instantiates with the default value */


	/* Add the node */
	UA_NodeId myv_NodeId = UA_NODEID_STRING(1, "frame Variable");
	UA_QualifiedName myv_NodeI_q = UA_QUALIFIEDNAME(1, "frame.Variable");
	UA_Server_addVariableNode(server, myv_NodeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		myv_NodeI_q, pointTypeId,
		vAttr, NULL, NULL);


}

/**
* Now we change the value with the write service. This uses the same service
* implementation that can also be reached over the network by an OPC UA client.
*/

static void
writeVariable(UA_Server *server) {
	UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");

	 myInteger++;
	UA_Variant myVar;
	UA_Variant_init(&myVar);
	UA_Variant_setScalar(&myVar, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
	UA_Server_writeValue(server, myIntegerNodeId, myVar);

	UA_WriteValue wv;
	UA_WriteValue_init(&wv);
	wv.nodeId = myIntegerNodeId;
	wv.attributeId = UA_ATTRIBUTEID_VALUE;
	wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
	wv.value.hasStatus = true;
	UA_Server_write(server, &wv);

	/* Reset the variable to a good statuscode with a value */
	wv.value.hasStatus = false;
	wv.value.value = myVar;
	wv.value.hasValue = true;
	UA_Server_write(server, &wv);

	///////////////////////////////////////////////////////////////////////////
	UA_NodeId frameNodeId = UA_NODEID_STRING(1, "frame Variable");
	UA_Variant_init(&myVar);
	UA_Byte picture[frame_pixel];
	cap >> img;
	imshow("Display window", img);
	waitKey(1);
	UA_Variant_setArray(&myVar , img.data, frame_pixel, &UA_TYPES[UA_TYPES_BYTE]);
	UA_Server_writeValue(server, frameNodeId, myVar);

	UA_WriteValue_init(&wv);
	wv.nodeId = frameNodeId;
	wv.attributeId = UA_ATTRIBUTEID_VALUE;
	wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
	wv.value.hasStatus = true;
	UA_Server_write(server, &wv);

	/* Reset the variable to a good statuscode with a value */
	wv.value.hasStatus = false;
	wv.value.value = myVar;
	wv.value.hasValue = true;
	UA_Server_write(server, &wv);

	////////////////////////////////////////////////////////////////////////////

}



UA_Boolean running = true;
static void stopHandler(int sign) {
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
	running = false;
}
void network() {
	cap >> img;
	namedWindow("Display window", WINDOW_AUTOSIZE);
	signal(SIGINT, stopHandler);
	signal(SIGTERM, stopHandler);

	UA_ServerConfig *config = UA_ServerConfig_new_default();
	UA_Server *server = UA_Server_new(config);
	addVariableTypeframe(server);
	addVariable(server);
	writeVariable(server);
	UA_Boolean waitInternal = false;
	UA_StatusCode retval = UA_Server_run_startup(server);
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Server_delete(server);
		UA_ServerConfig_delete(config);
		return;
	}
	int count = 0;
	while (running) {
		UA_UInt16 timeout = UA_Server_run_iterate(server, waitInternal);
		count++;
		writeVariable(server);
		UA_Variant value; /* Variants can hold scalar values and arrays of any type */
		UA_Variant_init(&value);
		UA_NodeId nodeId = UA_NODEID_STRING(1, "go");
		UA_Server_readValue(server, nodeId, &value);
		UA_Int32 v = *(UA_Int32 *)value.data;
		cout << v << endl;
	}
	retval = UA_Server_run_shutdown(server);
	UA_Server_delete(server);
	UA_ServerConfig_delete(config);
	return ;
}
int main(void) {
	
			thread t(network);
			system("pause");
			running = 0;
			t.join();
	return 0;
}

