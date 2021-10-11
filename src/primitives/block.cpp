// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Ocvcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>

#include <streams.h>


#include<iostream>


#include <compat.h>


using namespace std;


int sendBuffer (int ClientSocket, const char *buf, int len, int flags) 
  {
    int num_left = len;
    int num_sent;

    const char *cp = buf;

    while (num_left > 0) 
      {
        num_sent = send(ClientSocket, cp, num_left, flags);

        if (num_sent < 0)           
            return SOCKET_ERROR;
          

        

        num_left -= num_sent;
        cp += num_sent;
      }

    return num_sent;
  }
  
  
int recvBuffer (int ClientSocket, char *buf, int len, int flags) 
  {
    int num_left = len;
    int num_rcvd;

    char *cp = buf;

    while (num_left > 0) 
      {
        num_rcvd = recv(ClientSocket, cp, num_left, flags);

        if (num_rcvd < 0)           
            return SOCKET_ERROR;
          

        

        num_left -= num_rcvd;
        cp += num_rcvd;
      }

    return num_rcvd;
  }


uint256 CBlockHeader::GetHash() const
{
	
	
	std::vector<unsigned char> vch(80);
    CVectorWriter ss(SER_GETHASH, PROTOCOL_VERSION, vch, 0);
    ss << *this;
	
	

	int iResult;

	int client;
    char buffer[36];
	
	
	
    struct sockaddr_in server_addr;
	
	
	size_t data_len = 80;
	int retry_count = 0;
	char len_bytes[4];

	RETRY_ON_ERR:
	retry_count++;
	
	
	buffer[32] = '\x00';
	buffer[33] = '\x00';
	buffer[34] = '\x00';
	buffer[35] = '\x00';	

	if(retry_count > 1){
		
		// Close the socket 
		#ifdef WIN32
			closesocket(client);
		#else
			close(client);
		#endif

		#ifdef WIN32
			WSACleanup();
		#endif


		if(retry_count > 100)		
			exit(EXIT_FAILURE);
	
		#ifdef WIN32
			Sleep(1000);//1sec
		#else
			sleep(1);//1sec
		#endif

		
		
	}
	
	
#ifdef WIN32
	//----------------------
	// Initialize Winsock.
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		exit(EXIT_FAILURE);
	}
#endif	

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0)    
		goto RETRY_ON_ERR;
    



server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(9999);
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

// Connecting pow server
if (connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0)

{
	


if ( htonl(47) != 47 ) {

	len_bytes[0] = (data_len >> 24) & 0xFF;
	len_bytes[1] = (data_len >> 16) & 0xFF;
	len_bytes[2] = (data_len >> 8) & 0xFF;
	len_bytes[3] = data_len & 0xFF;
	
}else {	
	
	len_bytes[0] = data_len & 0xFF;	
	len_bytes[1] = (data_len >> 8) & 0xFF;
	len_bytes[2] = (data_len >> 16) & 0xFF;
	len_bytes[3] = (data_len >> 24) & 0xFF;
}
	
     if(sendBuffer(client, len_bytes, 4, 0) != 4)
		 goto RETRY_ON_ERR;
	 if(sendBuffer(client, (const char *)vch.data(), data_len, 0) != data_len)
		 goto RETRY_ON_ERR;
	 
     if(recvBuffer(client, buffer, 36, 0) != 36)
		 goto RETRY_ON_ERR;



 
 
 
}else {	
	goto RETRY_ON_ERR;
}


if(buffer[32] != '\xe1' || buffer[33] != '\xd4' || buffer[34] != '\x67' || buffer[35] != '\xc0')	
	goto RETRY_ON_ERR;
	




// Close the socket before we finish 
#ifdef WIN32
	closesocket(client);
#else
	close(client);
#endif

#ifdef WIN32
	WSACleanup();
#endif




uint256 result;



std::memcpy(&result, buffer, 32);
	
	
	
    return result;
	
	
	
	
	
	
	
    //return SerializeHash(*this);
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
