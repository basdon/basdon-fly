/**
 * @param rpcdata an array of structs in which the message rpcdata will be filled
 * @param amount of entries in the rpcdata parameter array
 * returns number indicating how many of rpcdata parameter have been filled to split the message
 */
static
int util_splitclientmessage(struct RPCDATA_SendClientMessage *rpcdata, int num_rpcdata, int color, char *message)
{
	TRACE;
	int i, j;
	int num_packets;
	char *msg_start;
	int msglen;
	int cut_pos, min_cut_pos;
	int pos_inc;
	char colorpart;
	int newcolor;

	msglen = strlen(message);

	/*if it fits in one, it fits in one*/
	if (msglen < 145) {
		rpcdata[0].color = color;
		rpcdata[0].message_length = msglen;
		memcpy(rpcdata[0].message, message, msglen);
		return 1;
	}

	num_packets = 0;
	msg_start = message;

	while (msglen > 0 && num_packets < num_rpcdata) {
		if (msglen > 144) {
			/*Try to split on whitespace or color boundaries.*/
			for (cut_pos = 144, min_cut_pos = 144 - 15; cut_pos > min_cut_pos; cut_pos--) {
				if (msg_start[cut_pos] == ' ') {
					pos_inc = cut_pos + 1;
					goto have_cut_pos;
				} else if (cut_pos <= msglen - 7 && msg_start[cut_pos] == '{' && msg_start[cut_pos + 7] == '}') {
					pos_inc = cut_pos;
					goto have_cut_pos;
				}
			}
			cut_pos = pos_inc = 144;
		} else {
			cut_pos = pos_inc = msglen;
		}
have_cut_pos:
		rpcdata[num_packets].color = color;
		rpcdata[num_packets].message_length = cut_pos;
		memcpy(rpcdata[num_packets].message, msg_start, cut_pos);
		num_packets++;
		/*Find the last embedded color to use for the next packet.*/
		for (i = cut_pos - 1; i >= 7; i--) {
			if (msg_start[i] == '}' && msg_start[i - 7] == '{') {
				newcolor = color & 0xFF;
				for (j = 0; j < 6; j++) {
					colorpart = msg_start[i - 6 + j];
					if ('0' <= colorpart && colorpart <= '9') {
						newcolor |= (colorpart - '0') << (28 - j * 4);
					} else if ('a' <= colorpart && colorpart <= 'f') {
						newcolor |= (colorpart - 'a' + 10) << (28 - j * 4);
					} else if ('A' <= colorpart && colorpart <= 'F') {
						newcolor |= (colorpart - 'A' + 10) << (28 - j * 4);
					} else {
						goto not_a_color;
					}
				}
				color = newcolor;
				break;
			}
not_a_color:
			;
		}
		msglen -= pos_inc;
		msg_start += pos_inc;
	}

	return num_packets;
}
