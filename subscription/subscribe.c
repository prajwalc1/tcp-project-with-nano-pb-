#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "inotification.pb.h"
#include <iphlpapi.h>
#include <ws2def.h>
#include "common.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define REQUEST_RESPONSE 674
#define MAGIC_NUMBER 0x5AA5
#define INOTIFICATION_ID 120
#define RHMI_ID 1007
#define DEFAULT_BUFLEN 100
#define DEFAULT_PORT "9002"

const uint16_t HEART_BEAT_SUBSCRIPTION[1] = { 100};

typedef struct notification_subscribe_header_s{
	uint16_t Service_ID;
	uint16_t Instance_ID;
	uint16_t Function_ID;
	uint16_t OpType;
	uint16_t Client_Handle;
	uint16_t Magic_Number;
	uint32_t Reserved;
	uint32_t Payload_Length;

}notification_subscribe_header_t;


static bool ucCompile_HeartBeart_Subscription_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
	uint16_t uhIndex;
	bool result;
	(void)(*arg);
	uhIndex = 0;
	result = false;
	result = pb_encode_tag_for_field(stream, field);
	if (false != result)
	{
		
		for (uhIndex = 0; uhIndex < sizeof(HEART_BEAT_SUBSCRIPTION) / sizeof(HEART_BEAT_SUBSCRIPTION[0]); uhIndex++)
		{
			result = pb_encode_varint(stream, HEART_BEAT_SUBSCRIPTION[uhIndex]);
		}
		//pb_encode_string(stream, HEART_BEAT_SUBSCRIPTION, sizeof(HEART_BEAT_SUBSCRIPTION) + 1);
	}
	return result;

}
functionResults funcresults[20] = functionResults_init_zero;
bool results_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
	while (stream->bytes_left)
	{
		if (!pb_decode(stream, functionResults_fields, &funcresults))
			return false;

		//printf("%X %b %b %b \n",funcresults.FunctionID, funcresults.has_FunctionID,funcresults.has_Successful,funcresults.Successful);
	}

	return true;
}
int main(int argc, char **argv)
{
	uint8_t result;
	notification_subscribe_header_t header1;
	int i,j,k;
	uint8_t swap;
	result = false;
	header1.Service_ID = RHMI_ID;
	header1.Instance_ID = 1;
	header1.Function_ID = INOTIFICATION_ID;
	header1.OpType = REQUEST_RESPONSE;
	header1.Client_Handle = 100;
	header1.Magic_Number = MAGIC_NUMBER;

	FILE *subscribe_file, *text_file;
	errno_t err, err1;
	
	/*socket data*/

	uint8_t sendbuf[80];
	uint8_t recvbuf[DEFAULT_BUFLEN];
	int iResult;
	bool status;
	int recvbuflen = DEFAULT_BUFLEN;
	WSADATA wsaData;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// IP address, and port of the server to be connected to.
	struct sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("192.168.1.100");
	clientService.sin_port = htons(9002);
	// Initialize Winsock
	
	iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	wprintf(L"Connected to server.\n");

	/*init Notification request*/
	Subscribe_request_response HeartBeartRequest = Subscribe_request_response_init_default;
	HeartBeartRequest.FunctionID.funcs.encode = &ucCompile_HeartBeart_Subscription_callback;
	HeartBeartRequest.FunctionID.arg = (void*)(&HEART_BEAT_SUBSCRIPTION);

	

	uint8_t buffer[1024]; /* here we can use the PDU Struct (header plus fixed array field)*/
	memset(buffer, 0x00, sizeof(buffer));
	/****************************************************************/

	pb_ostream_t stream;

	stream = pb_ostream_from_buffer(&buffer[0]+20, 1024 );
	//stream = pb_ostream_from_buffer(&buffer[0], 1024);
	if (!pb_encode(&stream, Subscribe_request_response_fields, &HeartBeartRequest))
	{
		result = false;
		printf_s("Encoding failed: %s\n", PB_GET_ERROR(&stream));
		return 1;
	}
	header1.Payload_Length = stream.bytes_written;
	//memcpy(&buffer[header1.Payload_Length], &header1, sizeof(header1));
	memcpy(buffer, &header1, sizeof(header1));
	err = fopen_s(&subscribe_file, "subscribe.bin", "wb");
	err1 = fopen_s(&text_file, "subscribe_1.txt", "wb");
	if (err || err1 != 0)
	{
		printf("unable to open file\n");
		return 1;
	}
	for (i = 0; i < 20; i=i+2)
	{
		swap = buffer[i];
		buffer[i] = buffer[i + 1];
		buffer[i + 1] = swap;
	}
	swap = buffer[19];
	buffer[19] = buffer[17];
	buffer[17] = swap;
	//for (i = 0; i < 22; i = i + 1)
	//{
	//	sendbuf[i] = buffer[i];
	//}
	// Send an initial buffer
	iResult = send(ConnectSocket, buffer, header1.Payload_Length + sizeof(header1), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);
	fwrite(buffer, header1.Payload_Length + sizeof(header1), 1, subscribe_file);
	fwrite(buffer, header1.Payload_Length + sizeof(header1), 1, text_file);
	fclose(subscribe_file);
	fclose(text_file);


	///to decode the response 
	//Subscribe_request_response HeartBeartResponse = Subscribe_request_response_init_default;
	//uint8_t recv_buffer[1024]; /* here we can use the PDU Struct (header plus fixed array field)*/
	//memset(recv_buffer, 0x00, sizeof(recv_buffer));

	//// receive stream 
	//bool(*decode)(pb_istream_t *stream2, const pb_field_t *field, void **arg);
	//pb_istream_t stream2 = pb_istream_from_buffer(&recv_buffer,1024);

	//decode stream using callback

	Subscribe_response heartbeatresponse = Subscribe_response_init_zero;
	functionResults results = functionResults_init_default;
	
	//pb_istream_t input = pb_istream_from_socket(ConnectSocket);

	/* Give a pointer to our callback function, which will handle the
	* filenames as they arrive. */
	/*heartbeatresponse.FunctionResults.funcs.decode = &results_callback;

	if (!pb_decode(&input, Subscribe_request_response_fields, &heartbeatresponse))
	{
		fprintf(stderr, "Decode failed: %s\n", PB_GET_ERROR(&input));
		return false;
	}*/



	notification_subscribe_header_t header2;
	// Receive until the peer closes the connection
	bool checked = false;
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

		for (k = 20; k < sizeof(recvbuf);k++)
		{
			if (recvbuf[k] != NULL)
			{
				printf("%X", recvbuf[k]);
			}
		}
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());
		memcpy(&header2, &recvbuf, sizeof(header2));
		printf("\n%0X,%0X,%0X,%0X \n", header2.Function_ID, header2.Instance_ID, header2.Service_ID, header2.Payload_Length);
		if (!checked)
		{
			checked = true;
			memcpy(&header2, &recvbuf,sizeof(header2));
			printf("\n%0X,%0X,%0X,%0X \n",header2.Function_ID,header2.Instance_ID,header2.Service_ID,header2.Payload_Length );
			pb_istream_t input = pb_istream_from_buffer(&recvbuf[20], 10);

			/* Give a pointer to our callback function, which will handle the
			* filenames as they arrive. */
			heartbeatresponse.FunctionResults.funcs.decode = &results_callback;

			if (!pb_decode(&input, Subscribe_response_fields, &heartbeatresponse))
			{
				fprintf(stderr, "Decode failed: %s\n", PB_GET_ERROR(&input));
				return false;
			}
		}
	} while (iResult > 0);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 1;
}



