/*
 * simp_broker.h
 *
 *  Created on: 23.03.2019
 *      Author: tomek
 */

#ifndef BROKER_SIMP_BROKER_H_
#define BROKER_SIMP_BROKER_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>


#define X_HTONS(a) 					((a>>8) | (a<<8))
#define PCK_TYPE_POS				(4)


namespace Mqtt{
using TopicName = std::string;
using  PacketType = uint8_t;
constexpr PacketType connect	 = 1;
constexpr PacketType connack     = 2;
constexpr PacketType publish     = 3;
constexpr PacketType puback	     = 4;
constexpr PacketType pubrec	     = 5;
constexpr PacketType pubrel	     = 6;
constexpr PacketType pubcomp	 = 7;
constexpr PacketType subscribe   = 8;
constexpr PacketType suback	     = 9;
constexpr PacketType unsubscribe = 10;
constexpr PacketType unsuback	 = 11;
constexpr PacketType pingreq	 = 12;
constexpr PacketType pingresp    = 13;
constexpr PacketType disconnect  = 14;
}



template <typename T>
std::string to_string(T value) //due to compiler issue
{
	//create an output string stream
	std::ostringstream os ;

	//throw the value into the string stream
	os << value ;

	//convert the string stream into a string and return
	return os.str() ;
}



class Client{
	;
};

typedef struct{
	uint8_t reserved :4;
	uint8_t type :4;
}ping_ctrl_byte_t;

struct pub_ctrl_byte_t{
	uint8_t retain :1;
	uint8_t QoS :2;
	uint8_t dup :1;
	uint8_t type :4;
};

struct sub_ctrl_byte_t{
	uint8_t reserved :4;
	uint8_t type :4;
};


struct con_ctrl_byte_t{
	uint8_t reserved :4;
	uint8_t type :4;
};



class ctrl_byte_t{
	uint8_t reserved :4;
	uint8_t type :4;
};







struct rem_length_t{
	uint32_t value;
	uint8_t bytes_nb;
};


inline rem_length_t decode_pck_len (char * frame){
	uint8_t multiplier = 1;
	uint8_t  encodedByte;
	rem_length_t rem_length;
	memset(&rem_length, 0, sizeof(rem_length_t));
	const uint8_t max_nb_bytes = 4;
	do{
		encodedByte = frame[rem_length.bytes_nb];
		rem_length.value += (encodedByte & 127) * multiplier;
		multiplier *= 128;
		rem_length.bytes_nb++;
		if (rem_length.bytes_nb == max_nb_bytes){
			break;
		}
	}while ((encodedByte & 128) != 0);
	return rem_length;
}


class VariableHead{
public:
	uint16_t len;
	std::string name;
};

class PubVarHead : public VariableHead{
public:
	uint16_t packet_id;
};


class SubVarHead : public VariableHead{
public:
	uint16_t * packet_id;
};



struct conn_flags_t{
	uint8_t reserved 	   :1;
	uint8_t cleans_session :1;
	uint8_t last_will      :1;
	uint8_t will_qos       :2;
	uint8_t will_retain    :1;
	uint8_t pswd           :1;
	uint8_t user_name      :1;
};

class ConVarHead : public VariableHead{
public:
	uint8_t * proto_level;
	conn_flags_t conn_flags;
	uint16_t * keep_alive;
};

class FixHead{
	ctrl_byte_t ctrl_byte;
public:
	uint32_t rem_len;
};


class PubFixHead : public FixHead{
public:
	pub_ctrl_byte_t pub_ctrl_byte;
};



class SubFixHead : public FixHead{
public:
	sub_ctrl_byte_t sub_ctrl_byte;
};


class ConFixHead : public FixHead{
public:
	con_ctrl_byte_t con_ctrl_byte;
};






class Packet{
public:
	FixHead fix_head;
	VariableHead var_head;
	std::string ID;
	virtual void Deserialize(char* frame) = 0;
	virtual void Process(void) =0;
};


class SubsPacket : public Packet{
	SubFixHead fix_head;
	SubVarHead var_head;
	std::string payload;
	Mqtt::TopicName GetTopicName(void);
	void Deserialize(char *frame){
		uint8_t pos = 0;

		sub_ctrl_byte_t * ctr_byte_ptr = (sub_ctrl_byte_t*) frame;
		fix_head.sub_ctrl_byte = *ctr_byte_ptr;
		pos++;
		rem_length_t rem_length = decode_pck_len(&frame[pos]);
		fix_head.rem_len = rem_length.value;
		pos += rem_length.bytes_nb;


		var_head.packet_id  = (uint16_t*) &frame[pos];
		*var_head.packet_id = X_HTONS(*var_head.packet_id);
		pos += 2;

		const uint8_t fix_head_size = 2;
		uint8_t topic_nb =0;

		payload = to_string(&frame[pos]);
		//		while (pos < (fix_head.rem_len + fix_head_size)){
		//			pld_topics[topic_nb].len = (uint16_t *)  &frame[pos];
		//			*pld_topics[topic_nb].len  = X_HTONS(*pld_topics[topic_nb].len );
		//			pos += 2;
		//			pld_topics[topic_nb].name =  (char*)  &frame[pos];
		//			pos += (*pld_topics[topic_nb].len);
		//			pld_topics[topic_nb].qos = (uint8_t*) &frame[pos];
		//			pos += 1;
		//			topic_nb++;
		//		}
	}
	void Process(void){
		;
	}
};




class PubPacket : public Packet{
	PubFixHead fix_head;
	PubVarHead var_head;
	std::string payload;
public:
	void Deserialize(char *frame){

		uint8_t pos = 0;
		pub_ctrl_byte_t * ctr_byte_ptr = (pub_ctrl_byte_t*) &frame[0];
		fix_head.pub_ctrl_byte = *ctr_byte_ptr;
		pos ++;

		rem_length_t rem_length = decode_pck_len(&frame[pos]);
		fix_head.rem_len = rem_length.value;
		pos +=  rem_length.bytes_nb;

		uint16_t * len_ptr  = (uint16_t*) &frame[pos];
		var_head.len = X_HTONS(*len_ptr);
		pos += var_head.len;

		var_head.name = to_string(&frame[pos]);
		pos += var_head.name.length();

		if (fix_head.pub_ctrl_byte.QoS > 0){
			uint16_t * packet_id_ptr  =  (uint16_t*) &frame[pos];
			var_head.packet_id = X_HTONS(*packet_id_ptr);
			pos += 2;
		}
		payload = to_string(&frame[pos]);
	}
	void Process(void){
		;
	}

};



//	Mqtt::TopicName GetTopicName(void);
//};


//  mosquitto_sub -h localhost -p 1886 -u user -P pass -v -t 'devices/#' -v -d -V mqttv311

class ConnPacket : public Packet{
	ConFixHead fix_head;
	ConVarHead var_head;
	std::string payload;
	void Deserialize(char* frame){
		uint8_t pos = 0;
		con_ctrl_byte_t * ctrl_byte =  (con_ctrl_byte_t *) &frame[pos];
		fix_head.con_ctrl_byte =  *ctrl_byte;
		pos ++;

		rem_length_t rem_length = decode_pck_len(&frame[pos]);
		fix_head.rem_len = rem_length.value;
		pos += rem_length.bytes_nb;

		uint8_t nameLenMsb = frame[pos++];
		uint8_t nameLenLsb = frame[pos++];

		uint16_t nameLen = nameLenMsb + nameLenLsb;
		var_head.name = to_string(&frame[pos]);
		pos += var_head.name.length();
		var_head.proto_level = (uint8_t*) &frame[pos];//8
		pos += 1;
		var_head.conn_flags = *(conn_flags_t*) &frame[pos];//9
		pos += 1;
		var_head.keep_alive = (uint16_t*)  &frame[pos];//10+11
		*var_head.keep_alive = X_HTONS(*var_head.keep_alive);
		pos += 2;

		payload = to_string(&frame[pos]);
		//		pld.client_id_len  = (uint16_t*) &frame[pos];//1213
		//		*pld.client_id_len = X_HTONS(*pld.client_id_len);
		//		pos += 2;
		//		pld.client_id = (char*) &frame[pos];
		//		pos += *pld.client_id_len;
		//
		//		if (var_head.conn_flags->last_will){
		//			pld.will_topic_len = (uint16_t*)  &frame[pos];
		//			*pld.will_topic_len = X_HTONS(* pld.will_topic_len);
		//			pos += 2;
		//			pld.will_topic = (char*)  &frame[pos];
		//			pos += *pld.will_topic_len;
		//
		//			pld.will_msg_len = (uint16_t*)  &frame[pos];
		//			*pld.will_msg_len = X_HTONS(* pld.will_msg_len);
		//			pos += 2;
		//			pld.will_msg = (char*)  &frame[pos];
		//			pos += *pld.will_msg_len;
		//		}
		//		if (var_head.conn_flags->user_name){
		//			pld.usr_name_len = (uint16_t*)  &frame[pos];
		//			*pld.usr_name_len = X_HTONS(* pld.usr_name_len);
		//			pos += 2;
		//			pld.usr_name= (char*) &frame[pos];
		//			pos += *pld.usr_name_len;
		//		}
		//		if (var_head.conn_flags->pswd){
		//			pld.pswd_len = (uint16_t*)  &frame[pos];
		//			*pld.pswd_len = X_HTONS(* pld.pswd_len);
		//			pos += 2;
		//			pld.pswd= (char*) &frame[pos];
		//			pos += *pld.pswd_len;
		//		}
	}
	void Process(void){
		;
	}
};




inline Mqtt::PacketType GetPacketType(char * frame){
	return (Mqtt::PacketType) (frame[0]>>PCK_TYPE_POS);
}



class SimpBroker{
	std::map<Mqtt::PacketType, Packet*> factoryMap;
	std::vector<Client> connectedClients;
	std::map<Mqtt::TopicName, std::vector<Client>> subscriptions;

	//18124
public:


	SimpBroker() {
		factoryMap[Mqtt::connect] = new ConnPacket();
		factoryMap[Mqtt::publish] = new PubPacket;
		factoryMap[Mqtt::subscribe] = new SubsPacket;

	}


	Packet *PacketFactory(Mqtt::PacketType packetType){
		return factoryMap[packetType];
	}



	void OnReceivedFrame(char * frame, std::string address){
		Mqtt::PacketType packetType = GetPacketType(frame);
		Packet * packet = PacketFactory(packetType);
		packet->Deserialize(frame);
		packet->Process();
		//Packet * packet = deserializeMap[packetType]->Deserialize(frame);
		//processMap[packetType]->Process(packet, address);
	}

};



#endif /* BROKER_SIMP_BROKER_H_ */
