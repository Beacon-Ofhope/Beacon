#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>

#include "../Includes/_time.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"


Bobject* mktime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate) {
	if (!args || BARG[0]->type != BINTERFACE)
		return _type_error(bstate, "Time.mktime() takes 1 argument (time_interface).");

	struct tm when = mk_tm_struct(BARG[0]->d, bstate);
	return mk_number(mktime(&when), bstate);
}

Bobject* localtime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate) {
	if (!args || BARG[0]->type != BNUMBER)
		return _type_error(bstate, "Time.localtime() takes 1 number argument (time_t).");

	time_t arg = (time_t)(BARG[0]->value.num_value);
	return mk_bn_tm_struct(localtime(&arg), bstate);
}

Bobject* gmtime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate) {
	if (!args || BARG[0]->type != BNUMBER)
		return _type_error(bstate, "Time.gmtime() takes 1 number argument (time_t).");

	time_t arg = (time_t)(BARG[0]->value.num_value);
	return mk_bn_tm_struct(gmtime(&arg), bstate);
}

Bobject* asctime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate) {
	if (!args || BARG[0]->type != BINTERFACE)
		return _type_error(bstate, "Time.asctime() takes 1 argument (time_interface).");

	struct tm when = mk_tm_struct(BARG[0]->d, bstate);

	char* date = malloc(sizeof(26));
	char* ret = asctime_r(&when, date);

	if (ret == NULL){
		free(date);
		return _sys_error(bstate, errno);
	}

	return mk_string(date, bstate);
}

Bobject* strftime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate) {
	if (!args || args->value.num_value < 2)
		return _type_error(bstate, "Time.strftime() takes 2 arguments (fmt_string, time_interface).");

	Bobject** arg = BARG;

	if (arg[0]->type != BSTRING || arg[1]->type != BINTERFACE)
		return _type_error(bstate, "Time.strftime() takes 2 arguments (fmt_string, time_interface).");

	struct tm when = mk_tm_struct(arg[1]->d, bstate);
	char* fmt = arg[0]->value.str_value;

	size_t len = strlen(fmt) + 1024;

	char* date = malloc(len);
	len = strftime(date, len, fmt, &when);

	if (len == 0){
		free(date);
		return _sys_error(bstate, errno);
	}

	date = realloc(date, strlen(date)+1);
	date[strlen(date)] = '\0';

	return mk_string(date, bstate);
}

Bobject* strptime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate) {
	// if (!args || args->value.num_value == 0)
	// 	return _type_error(bstate, "Time.strptime() takes 1 argument (date_string) or 2 (.., fmt_string).");

	// Bobject* arg = BARG;

	// if (arg[0].type != BSTRING)
	// 	return _type_error(bstate, "Time.strptime() takes 1 argument (date_string) or 2 (.., fmt_string).");

	// char *ret;
	// struct tm when;
	// char* data = arg[0].value.str_value;

	// if (args->value.num_value == 1){
	// 	ret = strptime(data, "%F", &when);

	// 	if (ret == NULL)
	// 		ret = strptime(data, "%D", &when);

	// } else {
	// 	ret = strptime(data, arg[1].value.str_value, &when);
	// }

	// if (ret == NULL)
	// 	return bstate->none;

	// return mk_bn_tm_struct(&when, bstate);
	return bstate->none;
}

Bobject* sleep_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	if (!args)
		return _type_error(bstate, "'Time.sleep' takes 1 number argument (seconds).");

	sleep((unsigned int)(BARG[0]->value.num_value));
	return bstate->none;
}

Bobject* diff_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	if (!args || args->value.num_value < 2)
		return _type_error(bstate, "'Time.difftime' takes 2 number arguments (time_1, time_2).");

	Bobject** arg = BARG;
	return mk_number(difftime(arg[0]->value.num_value, arg[1]->value.num_value), bstate);
}

Bobject* ctime_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	time_t tm;

	if (!args || BARG[0]->type != BNUMBER)
		tm = time(NULL);
	else
		tm = (time_t)(BARG[0]->value.num_value);

	char* date = ctime(&tm);
	return mk_string(strdup(date), bstate);
}

Bobject* clock_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	return mk_number(clock(), bstate);
}

Bobject* time_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	return mk_number(time(NULL), bstate);
}

Bobject* tzname_time_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	if (tzname == NULL || (sizeof(tzname)/sizeof(char*)) < 1)
		return bstate->none;

	Bobject* tz_list = malloc(sizeof(Bobject)*2);
	tz_list[0] = *(mk_string(strdup(tzname[0]), bstate));
	tz_list[1] = *(mk_string(strdup(tzname[1]), bstate));

	return mk_mod_list(2, tz_list, bstate);
}


Bobject *Bn_Time() {
    Stack *M = create_stack(15);

	add_to_stack(M, "asctime", mk_module_fun("[Function Time.asctime]", asctime_time_bn, NULL));
	add_to_stack(M, "clock", mk_module_fun("[Function Time.clock]", clock_time_bn, NULL));
	add_to_stack(M, "ctime", mk_module_fun("[Function Time.ctime]", ctime_time_bn, NULL));
	add_to_stack(M, "difftime", mk_module_fun("[Function Time.difftime]", diff_time_bn, NULL));
	add_to_stack(M, "gmtime", mk_module_fun("[Function Time.gmtime]", gmtime_time_bn, NULL));
	add_to_stack(M, "localtime", mk_module_fun("[Function Time.localtime]", localtime_time_bn, NULL));
	add_to_stack(M, "mktime", mk_module_fun("[Function Time.gmtime]", mktime_time_bn, NULL));
	add_to_stack(M, "sleep", mk_module_fun("[Function Time.sleep]", sleep_time_bn, NULL));
	add_to_stack(M, "strftime", mk_module_fun("[Function Time.strftime]", strftime_time_bn, NULL));
	add_to_stack(M, "strptime", mk_module_fun("[Function Time.strptime]", strptime_time_bn, NULL));
	add_to_stack(M, "time", mk_module_fun("[Function Time.time]", time_time_bn, NULL));
	add_to_stack(M, "tzname", mk_module_fun("[Function Time.timezone]", tzname_time_bn, NULL));

	bmodule *mod = malloc(sizeof(bmodule));
	mod->attrs = M;

	Bobject *bin = mk_safe_Bobject();
	bin->value.str_value = "[Module Time]";
	bin->type = BMODULE;
	bin->d = mod;

	return bin;
}
