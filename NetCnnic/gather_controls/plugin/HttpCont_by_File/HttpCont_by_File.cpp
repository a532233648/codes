
#include <unistd.h>
#include <zlib.h>

#include "control_plugin.h"
#include "HttpCont_by_File.h"
#include "plugin_inter.h"


#define	TEST_URL "www.sina.com"


/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzdecompress(Byte *zdata, uLong nzdata,
				 Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	static char dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in  = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
			//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
			//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if(err != Z_OK) {
			if(err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*) dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			} else return -1;
		}
	}
	if(inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}

void *Thread_Main(void *arg ){
	CONTROL_PLUGIN_INFO_T *pinfo = (CONTROL_PLUGIN_INFO_T*)arg;
	
	if(pinfo==NULL)
		return NULL;
		
	while(1){ 
		PLUGIN_INTER_STRUCT_T	result;
		
		if(pinfo->result_queue->Pop(&result)>0){
			if( result.task_rest_head.packet_type==PACKET_TYPE_REQUEST){
				result.task_rest_head.control_id = pinfo->id;
				result.task_rest_head.packet_type = PACKET_TYPE_TASK;
				result.task_rest_head.cont_size = sizeof(uint32_t)+strlen(TEST_URL)+1;
				result.content = (char *)realloc(result.content,result.task_rest_head.cont_size);
				strcpy(result.content+sizeof(uint32_t),TEST_URL);
			 	while(pinfo->task_queue->Push(&result)<=0){
					usleep(10);
				}
			}else if(result.task_rest_head.packet_type==PACKET_TYPE_RESULT){
			//	print_PLUGIN_INTER_STRUCT_T(&result);
				
				char *ptr = result.content;
				
				uint32_t	httpcode = ntohl(*((uint32_t*)ptr));
				uint32_t	clen = ntohl(*((uint32_t*)(ptr+4)));
				uint32_t	gzip = ntohl(*((uint32_t*)(ptr+8)));

				if(clen<=0){
					_LOG_NOTICE("\nhttpcode=%u,len=%u\n%s",httpcode,clen);
					Release_PLUGIN_INTER_STRUCT_T(&result);
					continue;
				}

				if(gzip){
					char *newcontent = (char *)malloc(clen*10);
					uLong	newlen=clen*10;
					if(newcontent && gzdecompress((Byte*)(ptr+12),clen,(Byte*)newcontent,(uLong*)&newlen)==0){
							newcontent[newlen]=0;
						if(newlen>100){
							newcontent[50]=0;
							newcontent[newlen-50]=0;
							_LOG_NOTICE("\nhttpcode=%u,len=%u,ungzip\n%s...%s",httpcode,newlen,newcontent,newcontent+newlen-40);
						}else{
							_LOG_NOTICE("\nhttpcode=%u,len=%u,ungzip\n%s",httpcode,newlen,newcontent);
						}
						free(newcontent);
					}else{
							ptr[clen-1]=0;
						if(clen>100){
							ptr[50]=0;
							ptr[clen-50]=0;
							_LOG_NOTICE("\nhttpcode=%u,len=%u,gzip\n%s...%s",httpcode,clen,ptr+12,ptr+clen-40);
						}else{
							_LOG_NOTICE("\nhttpcode=%u,len=%u,gzip\n%s",httpcode,clen,ptr+12);
						}  
					}
						
				}else{
							ptr[clen-1]=0;
						if(clen>100){
							ptr[50]=0;
							ptr[clen-50]=0;
							_LOG_NOTICE("\nhttpcode=%u,len=%u,deflate\n%s...%s",httpcode,clen,ptr+12,ptr+clen-40);
						}else{
							_LOG_NOTICE("\nhttpcode=%u,len=%u,deflate\n%s",httpcode,clen,ptr+12);
						} 
				}


				Release_PLUGIN_INTER_STRUCT_T(&result);
			}
		}else{
			usleep(1);
		}
	}
	
	return NULL;
}
