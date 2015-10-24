/*
 * Copyright (c) 2012, 2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _DZIMAGE_H_
#define _DZIMAGE_H_

#define ALIGN_SIZE 2048
#define MAX_NAME 512
#define MAGIC_NUM 0x54495A4E

struct dzimage_hdr {
	unsigned int magic_num;
	unsigned int kernel_addr;
	unsigned int kernel_size;
	unsigned int dt_addr;
	unsigned int dt_size;
	unsigned int tags_addr;
	unsigned int page_size;
	unsigned char cmdline[MAX_NAME];
};

#endif /* _DZIMAGE_H_ */