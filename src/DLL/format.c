//---[s]--- For InternetTime 99/03/16@211 M.Takemura -----

/*------------------------------------------------------------------------
// format.c : to make a string to display in the clock -> KAZUBON 1997-1998
//-----------------------------------------------------------------------*/
// Last Modified by Stoic Joker: Friday, 12/16/2011 @ 3:36:00pm
#include "tcdll.h"

static DWORD m_codepage = CP_ACP;
static wchar_t m_MonthShort[11], m_MonthLong[31];
static wchar_t m_DayOfWeekShort[11], m_DayOfWeekLong[31];
static wchar_t* m_DayOfWeekEng[7] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
static wchar_t* m_MonthEng[12] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
static wchar_t m_AM[10], m_PM[10];
static wchar_t m_EraStr[11];
static int m_AltYear;

typedef struct FORMAT_ZONE {
	const wchar_t* iana;
	const wchar_t* windows;
} FORMAT_ZONE;

typedef struct FORMAT_TZI {
	LONG bias;
	LONG standard_bias;
	LONG daylight_bias;
	SYSTEMTIME standard_date;
	SYSTEMTIME daylight_date;
} FORMAT_TZI;

static const FORMAT_ZONE m_format_zones[] = {
	// Generated from CLDR windowsZones.xml (IANA tz name -> Windows time zone
	// registry key name). Covers all standard Windows time zones so that any
	// commonly used IANA identifier can be selected in a custom format string.
	{L"UTC", L"UTC"},
	{L"Etc/UTC", L"UTC"},
	{L"Etc/GMT+12", L"Dateline Standard Time"},
	{L"Etc/GMT+11", L"UTC-11"},
	{L"Pacific/Pago_Pago", L"UTC-11"},
	{L"Pacific/Niue", L"UTC-11"},
	{L"Pacific/Midway", L"UTC-11"},
	{L"America/Adak", L"Aleutian Standard Time"},
	{L"Pacific/Honolulu", L"Hawaiian Standard Time"},
	{L"Pacific/Rarotonga", L"Hawaiian Standard Time"},
	{L"Pacific/Tahiti", L"Hawaiian Standard Time"},
	{L"Pacific/Marquesas", L"Marquesas Standard Time"},
	{L"America/Anchorage", L"Alaskan Standard Time"},
	{L"America/Juneau", L"Alaskan Standard Time"},
	{L"America/Metlakatla", L"Alaskan Standard Time"},
	{L"America/Nome", L"Alaskan Standard Time"},
	{L"America/Sitka", L"Alaskan Standard Time"},
	{L"America/Yakutat", L"Alaskan Standard Time"},
	{L"Etc/GMT+9", L"UTC-09"},
	{L"Pacific/Gambier", L"UTC-09"},
	{L"America/Tijuana", L"Pacific Standard Time (Mexico)"},
	{L"Etc/GMT+8", L"UTC-08"},
	{L"Pacific/Pitcairn", L"UTC-08"},
	{L"America/Los_Angeles", L"Pacific Standard Time"},
	{L"America/Vancouver", L"Pacific Standard Time"},
	{L"America/Phoenix", L"US Mountain Standard Time"},
	{L"America/Creston", L"US Mountain Standard Time"},
	{L"America/Dawson_Creek", L"US Mountain Standard Time"},
	{L"America/Fort_Nelson", L"US Mountain Standard Time"},
	{L"America/Hermosillo", L"US Mountain Standard Time"},
	{L"America/Mazatlan", L"Mountain Standard Time (Mexico)"},
	{L"America/Denver", L"Mountain Standard Time"},
	{L"America/Edmonton", L"Mountain Standard Time"},
	{L"America/Cambridge_Bay", L"Mountain Standard Time"},
	{L"America/Inuvik", L"Mountain Standard Time"},
	{L"America/Ciudad_Juarez", L"Mountain Standard Time"},
	{L"America/Boise", L"Mountain Standard Time"},
	{L"America/Whitehorse", L"Yukon Standard Time"},
	{L"America/Dawson", L"Yukon Standard Time"},
	{L"America/Guatemala", L"Central America Standard Time"},
	{L"America/Belize", L"Central America Standard Time"},
	{L"America/Costa_Rica", L"Central America Standard Time"},
	{L"Pacific/Galapagos", L"Central America Standard Time"},
	{L"America/Tegucigalpa", L"Central America Standard Time"},
	{L"America/Managua", L"Central America Standard Time"},
	{L"America/El_Salvador", L"Central America Standard Time"},
	{L"America/Chicago", L"Central Standard Time"},
	{L"America/Winnipeg", L"Central Standard Time"},
	{L"America/Rankin_Inlet", L"Central Standard Time"},
	{L"America/Resolute", L"Central Standard Time"},
	{L"America/Matamoros", L"Central Standard Time"},
	{L"America/Ojinaga", L"Central Standard Time"},
	{L"America/Indiana/Knox", L"Central Standard Time"},
	{L"America/Indiana/Tell_City", L"Central Standard Time"},
	{L"America/Menominee", L"Central Standard Time"},
	{L"America/North_Dakota/Beulah", L"Central Standard Time"},
	{L"America/North_Dakota/Center", L"Central Standard Time"},
	{L"America/North_Dakota/New_Salem", L"Central Standard Time"},
	{L"Pacific/Easter", L"Easter Island Standard Time"},
	{L"America/Mexico_City", L"Central Standard Time (Mexico)"},
	{L"America/Bahia_Banderas", L"Central Standard Time (Mexico)"},
	{L"America/Merida", L"Central Standard Time (Mexico)"},
	{L"America/Monterrey", L"Central Standard Time (Mexico)"},
	{L"America/Chihuahua", L"Central Standard Time (Mexico)"},
	{L"America/Regina", L"Canada Central Standard Time"},
	{L"America/Swift_Current", L"Canada Central Standard Time"},
	{L"America/Bogota", L"SA Pacific Standard Time"},
	{L"America/Rio_Branco", L"SA Pacific Standard Time"},
	{L"America/Eirunepe", L"SA Pacific Standard Time"},
	{L"America/Coral_Harbour", L"SA Pacific Standard Time"},
	{L"America/Guayaquil", L"SA Pacific Standard Time"},
	{L"America/Jamaica", L"SA Pacific Standard Time"},
	{L"America/Cayman", L"SA Pacific Standard Time"},
	{L"America/Panama", L"SA Pacific Standard Time"},
	{L"America/Lima", L"SA Pacific Standard Time"},
	{L"America/Cancun", L"Eastern Standard Time (Mexico)"},
	{L"America/New_York", L"Eastern Standard Time"},
	{L"America/Nassau", L"Eastern Standard Time"},
	{L"America/Toronto", L"Eastern Standard Time"},
	{L"America/Iqaluit", L"Eastern Standard Time"},
	{L"America/Detroit", L"Eastern Standard Time"},
	{L"America/Indiana/Petersburg", L"Eastern Standard Time"},
	{L"America/Indiana/Vincennes", L"Eastern Standard Time"},
	{L"America/Indiana/Winamac", L"Eastern Standard Time"},
	{L"America/Kentucky/Monticello", L"Eastern Standard Time"},
	{L"America/Louisville", L"Eastern Standard Time"},
	{L"America/Port-au-Prince", L"Haiti Standard Time"},
	{L"America/Havana", L"Cuba Standard Time"},
	{L"America/Indianapolis", L"US Eastern Standard Time"},
	{L"America/Indiana/Marengo", L"US Eastern Standard Time"},
	{L"America/Indiana/Vevay", L"US Eastern Standard Time"},
	{L"America/Grand_Turk", L"Turks And Caicos Standard Time"},
	{L"America/Asuncion", L"Paraguay Standard Time"},
	{L"America/Halifax", L"Atlantic Standard Time"},
	{L"Atlantic/Bermuda", L"Atlantic Standard Time"},
	{L"America/Glace_Bay", L"Atlantic Standard Time"},
	{L"America/Goose_Bay", L"Atlantic Standard Time"},
	{L"America/Moncton", L"Atlantic Standard Time"},
	{L"America/Thule", L"Atlantic Standard Time"},
	{L"America/Caracas", L"Venezuela Standard Time"},
	{L"America/Cuiaba", L"Central Brazilian Standard Time"},
	{L"America/Campo_Grande", L"Central Brazilian Standard Time"},
	{L"America/La_Paz", L"SA Western Standard Time"},
	{L"America/Antigua", L"SA Western Standard Time"},
	{L"America/Anguilla", L"SA Western Standard Time"},
	{L"America/Aruba", L"SA Western Standard Time"},
	{L"America/Barbados", L"SA Western Standard Time"},
	{L"America/St_Barthelemy", L"SA Western Standard Time"},
	{L"America/Kralendijk", L"SA Western Standard Time"},
	{L"America/Manaus", L"SA Western Standard Time"},
	{L"America/Boa_Vista", L"SA Western Standard Time"},
	{L"America/Porto_Velho", L"SA Western Standard Time"},
	{L"America/Blanc-Sablon", L"SA Western Standard Time"},
	{L"America/Curacao", L"SA Western Standard Time"},
	{L"America/Dominica", L"SA Western Standard Time"},
	{L"America/Santo_Domingo", L"SA Western Standard Time"},
	{L"America/Grenada", L"SA Western Standard Time"},
	{L"America/Guadeloupe", L"SA Western Standard Time"},
	{L"America/Guyana", L"SA Western Standard Time"},
	{L"America/St_Kitts", L"SA Western Standard Time"},
	{L"America/St_Lucia", L"SA Western Standard Time"},
	{L"America/Marigot", L"SA Western Standard Time"},
	{L"America/Martinique", L"SA Western Standard Time"},
	{L"America/Montserrat", L"SA Western Standard Time"},
	{L"America/Puerto_Rico", L"SA Western Standard Time"},
	{L"America/Lower_Princes", L"SA Western Standard Time"},
	{L"America/Port_of_Spain", L"SA Western Standard Time"},
	{L"America/St_Vincent", L"SA Western Standard Time"},
	{L"America/Tortola", L"SA Western Standard Time"},
	{L"America/St_Thomas", L"SA Western Standard Time"},
	{L"America/Santiago", L"Pacific SA Standard Time"},
	{L"America/St_Johns", L"Newfoundland Standard Time"},
	{L"America/Araguaina", L"Tocantins Standard Time"},
	{L"America/Sao_Paulo", L"E. South America Standard Time"},
	{L"America/Cayenne", L"SA Eastern Standard Time"},
	{L"Antarctica/Rothera", L"SA Eastern Standard Time"},
	{L"Antarctica/Palmer", L"SA Eastern Standard Time"},
	{L"America/Fortaleza", L"SA Eastern Standard Time"},
	{L"America/Belem", L"SA Eastern Standard Time"},
	{L"America/Maceio", L"SA Eastern Standard Time"},
	{L"America/Recife", L"SA Eastern Standard Time"},
	{L"America/Santarem", L"SA Eastern Standard Time"},
	{L"Atlantic/Stanley", L"SA Eastern Standard Time"},
	{L"America/Paramaribo", L"SA Eastern Standard Time"},
	{L"America/Buenos_Aires", L"Argentina Standard Time"},
	{L"America/Argentina/La_Rioja", L"Argentina Standard Time"},
	{L"America/Argentina/Rio_Gallegos", L"Argentina Standard Time"},
	{L"America/Argentina/Salta", L"Argentina Standard Time"},
	{L"America/Argentina/San_Juan", L"Argentina Standard Time"},
	{L"America/Argentina/San_Luis", L"Argentina Standard Time"},
	{L"America/Argentina/Tucuman", L"Argentina Standard Time"},
	{L"America/Argentina/Ushuaia", L"Argentina Standard Time"},
	{L"America/Catamarca", L"Argentina Standard Time"},
	{L"America/Cordoba", L"Argentina Standard Time"},
	{L"America/Jujuy", L"Argentina Standard Time"},
	{L"America/Mendoza", L"Argentina Standard Time"},
	{L"America/Godthab", L"Greenland Standard Time"},
	{L"America/Nuuk", L"Greenland Standard Time"},
	{L"America/Montevideo", L"Montevideo Standard Time"},
	{L"America/Punta_Arenas", L"Magallanes Standard Time"},
	{L"America/Coyhaique", L"Magallanes Standard Time"},
	{L"America/Miquelon", L"Saint Pierre Standard Time"},
	{L"America/Bahia", L"Bahia Standard Time"},
	{L"Etc/GMT+2", L"UTC-02"},
	{L"America/Noronha", L"UTC-02"},
	{L"Atlantic/South_Georgia", L"UTC-02"},
	{L"Atlantic/Azores", L"Azores Standard Time"},
	{L"America/Scoresbysund", L"Azores Standard Time"},
	{L"Atlantic/Cape_Verde", L"Cape Verde Standard Time"},
	{L"Europe/London", L"GMT Standard Time"},
	{L"Atlantic/Canary", L"GMT Standard Time"},
	{L"Atlantic/Faeroe", L"GMT Standard Time"},
	{L"Europe/Guernsey", L"GMT Standard Time"},
	{L"Europe/Dublin", L"GMT Standard Time"},
	{L"Europe/Isle_of_Man", L"GMT Standard Time"},
	{L"Europe/Jersey", L"GMT Standard Time"},
	{L"Europe/Lisbon", L"GMT Standard Time"},
	{L"Atlantic/Madeira", L"GMT Standard Time"},
	{L"Atlantic/Reykjavik", L"Greenwich Standard Time"},
	{L"Africa/Ouagadougou", L"Greenwich Standard Time"},
	{L"Africa/Abidjan", L"Greenwich Standard Time"},
	{L"Africa/Accra", L"Greenwich Standard Time"},
	{L"America/Danmarkshavn", L"Greenwich Standard Time"},
	{L"Africa/Banjul", L"Greenwich Standard Time"},
	{L"Africa/Conakry", L"Greenwich Standard Time"},
	{L"Africa/Bissau", L"Greenwich Standard Time"},
	{L"Africa/Monrovia", L"Greenwich Standard Time"},
	{L"Africa/Bamako", L"Greenwich Standard Time"},
	{L"Africa/Nouakchott", L"Greenwich Standard Time"},
	{L"Atlantic/St_Helena", L"Greenwich Standard Time"},
	{L"Africa/Freetown", L"Greenwich Standard Time"},
	{L"Africa/Dakar", L"Greenwich Standard Time"},
	{L"Africa/Lome", L"Greenwich Standard Time"},
	{L"Africa/Sao_Tome", L"Sao Tome Standard Time"},
	{L"Africa/Casablanca", L"Morocco Standard Time"},
	{L"Africa/El_Aaiun", L"Morocco Standard Time"},
	{L"Europe/Berlin", L"W. Europe Standard Time"},
	{L"Europe/Andorra", L"W. Europe Standard Time"},
	{L"Europe/Vienna", L"W. Europe Standard Time"},
	{L"Europe/Zurich", L"W. Europe Standard Time"},
	{L"Europe/Busingen", L"W. Europe Standard Time"},
	{L"Europe/Gibraltar", L"W. Europe Standard Time"},
	{L"Europe/Rome", L"W. Europe Standard Time"},
	{L"Europe/Vaduz", L"W. Europe Standard Time"},
	{L"Europe/Luxembourg", L"W. Europe Standard Time"},
	{L"Europe/Monaco", L"W. Europe Standard Time"},
	{L"Europe/Malta", L"W. Europe Standard Time"},
	{L"Europe/Amsterdam", L"W. Europe Standard Time"},
	{L"Europe/Oslo", L"W. Europe Standard Time"},
	{L"Europe/Stockholm", L"W. Europe Standard Time"},
	{L"Arctic/Longyearbyen", L"W. Europe Standard Time"},
	{L"Europe/San_Marino", L"W. Europe Standard Time"},
	{L"Europe/Vatican", L"W. Europe Standard Time"},
	{L"Europe/Budapest", L"Central Europe Standard Time"},
	{L"Europe/Tirane", L"Central Europe Standard Time"},
	{L"Europe/Prague", L"Central Europe Standard Time"},
	{L"Europe/Podgorica", L"Central Europe Standard Time"},
	{L"Europe/Belgrade", L"Central Europe Standard Time"},
	{L"Europe/Ljubljana", L"Central Europe Standard Time"},
	{L"Europe/Bratislava", L"Central Europe Standard Time"},
	{L"Europe/Paris", L"Romance Standard Time"},
	{L"Europe/Brussels", L"Romance Standard Time"},
	{L"Europe/Copenhagen", L"Romance Standard Time"},
	{L"Europe/Madrid", L"Romance Standard Time"},
	{L"Africa/Ceuta", L"Romance Standard Time"},
	{L"Europe/Warsaw", L"Central European Standard Time"},
	{L"Europe/Sarajevo", L"Central European Standard Time"},
	{L"Europe/Zagreb", L"Central European Standard Time"},
	{L"Europe/Skopje", L"Central European Standard Time"},
	{L"Africa/Lagos", L"W. Central Africa Standard Time"},
	{L"Africa/Luanda", L"W. Central Africa Standard Time"},
	{L"Africa/Porto-Novo", L"W. Central Africa Standard Time"},
	{L"Africa/Kinshasa", L"W. Central Africa Standard Time"},
	{L"Africa/Bangui", L"W. Central Africa Standard Time"},
	{L"Africa/Brazzaville", L"W. Central Africa Standard Time"},
	{L"Africa/Douala", L"W. Central Africa Standard Time"},
	{L"Africa/Algiers", L"W. Central Africa Standard Time"},
	{L"Africa/Libreville", L"W. Central Africa Standard Time"},
	{L"Africa/Malabo", L"W. Central Africa Standard Time"},
	{L"Africa/Niamey", L"W. Central Africa Standard Time"},
	{L"Africa/Ndjamena", L"W. Central Africa Standard Time"},
	{L"Africa/Tunis", L"W. Central Africa Standard Time"},
	{L"Asia/Amman", L"Jordan Standard Time"},
	{L"Europe/Bucharest", L"GTB Standard Time"},
	{L"Asia/Nicosia", L"GTB Standard Time"},
	{L"Asia/Famagusta", L"GTB Standard Time"},
	{L"Europe/Athens", L"GTB Standard Time"},
	{L"Asia/Beirut", L"Middle East Standard Time"},
	{L"Africa/Cairo", L"Egypt Standard Time"},
	{L"Europe/Chisinau", L"E. Europe Standard Time"},
	{L"Asia/Damascus", L"Syria Standard Time"},
	{L"Asia/Hebron", L"West Bank Standard Time"},
	{L"Asia/Gaza", L"West Bank Standard Time"},
	{L"Africa/Johannesburg", L"South Africa Standard Time"},
	{L"Africa/Bujumbura", L"South Africa Standard Time"},
	{L"Africa/Gaborone", L"South Africa Standard Time"},
	{L"Africa/Lubumbashi", L"South Africa Standard Time"},
	{L"Africa/Maseru", L"South Africa Standard Time"},
	{L"Africa/Blantyre", L"South Africa Standard Time"},
	{L"Africa/Maputo", L"South Africa Standard Time"},
	{L"Africa/Kigali", L"South Africa Standard Time"},
	{L"Africa/Mbabane", L"South Africa Standard Time"},
	{L"Africa/Lusaka", L"South Africa Standard Time"},
	{L"Africa/Harare", L"South Africa Standard Time"},
	{L"Europe/Kiev", L"FLE Standard Time"},
	{L"Europe/Kyiv", L"FLE Standard Time"},
	{L"Europe/Mariehamn", L"FLE Standard Time"},
	{L"Europe/Sofia", L"FLE Standard Time"},
	{L"Europe/Tallinn", L"FLE Standard Time"},
	{L"Europe/Helsinki", L"FLE Standard Time"},
	{L"Europe/Vilnius", L"FLE Standard Time"},
	{L"Europe/Riga", L"FLE Standard Time"},
	{L"Asia/Jerusalem", L"Israel Standard Time"},
	{L"Africa/Juba", L"South Sudan Standard Time"},
	{L"Europe/Kaliningrad", L"Kaliningrad Standard Time"},
	{L"Africa/Khartoum", L"Sudan Standard Time"},
	{L"Africa/Tripoli", L"Libya Standard Time"},
	{L"Africa/Windhoek", L"Namibia Standard Time"},
	{L"Asia/Baghdad", L"Arabic Standard Time"},
	{L"Europe/Istanbul", L"Turkey Standard Time"},
	{L"Asia/Riyadh", L"Arab Standard Time"},
	{L"Asia/Bahrain", L"Arab Standard Time"},
	{L"Asia/Kuwait", L"Arab Standard Time"},
	{L"Asia/Qatar", L"Arab Standard Time"},
	{L"Asia/Aden", L"Arab Standard Time"},
	{L"Europe/Minsk", L"Belarus Standard Time"},
	{L"Europe/Moscow", L"Russian Standard Time"},
	{L"Europe/Kirov", L"Russian Standard Time"},
	{L"Europe/Simferopol", L"Russian Standard Time"},
	{L"Africa/Nairobi", L"E. Africa Standard Time"},
	{L"Antarctica/Syowa", L"E. Africa Standard Time"},
	{L"Africa/Djibouti", L"E. Africa Standard Time"},
	{L"Africa/Asmera", L"E. Africa Standard Time"},
	{L"Africa/Addis_Ababa", L"E. Africa Standard Time"},
	{L"Indian/Comoro", L"E. Africa Standard Time"},
	{L"Indian/Antananarivo", L"E. Africa Standard Time"},
	{L"Africa/Mogadishu", L"E. Africa Standard Time"},
	{L"Africa/Dar_es_Salaam", L"E. Africa Standard Time"},
	{L"Africa/Kampala", L"E. Africa Standard Time"},
	{L"Indian/Mayotte", L"E. Africa Standard Time"},
	{L"Asia/Tehran", L"Iran Standard Time"},
	{L"Asia/Dubai", L"Arabian Standard Time"},
	{L"Asia/Muscat", L"Arabian Standard Time"},
	{L"Europe/Astrakhan", L"Astrakhan Standard Time"},
	{L"Europe/Ulyanovsk", L"Astrakhan Standard Time"},
	{L"Asia/Baku", L"Azerbaijan Standard Time"},
	{L"Europe/Samara", L"Russia Time Zone 3"},
	{L"Indian/Mauritius", L"Mauritius Standard Time"},
	{L"Indian/Reunion", L"Mauritius Standard Time"},
	{L"Indian/Mahe", L"Mauritius Standard Time"},
	{L"Europe/Saratov", L"Saratov Standard Time"},
	{L"Asia/Tbilisi", L"Georgian Standard Time"},
	{L"Europe/Volgograd", L"Volgograd Standard Time"},
	{L"Asia/Yerevan", L"Caucasus Standard Time"},
	{L"Asia/Kabul", L"Afghanistan Standard Time"},
	{L"Asia/Tashkent", L"West Asia Standard Time"},
	{L"Antarctica/Mawson", L"West Asia Standard Time"},
	{L"Asia/Oral", L"West Asia Standard Time"},
	{L"Asia/Almaty", L"West Asia Standard Time"},
	{L"Asia/Aqtau", L"West Asia Standard Time"},
	{L"Asia/Aqtobe", L"West Asia Standard Time"},
	{L"Asia/Atyrau", L"West Asia Standard Time"},
	{L"Asia/Qostanay", L"West Asia Standard Time"},
	{L"Indian/Maldives", L"West Asia Standard Time"},
	{L"Indian/Kerguelen", L"West Asia Standard Time"},
	{L"Asia/Dushanbe", L"West Asia Standard Time"},
	{L"Asia/Ashgabat", L"West Asia Standard Time"},
	{L"Asia/Samarkand", L"West Asia Standard Time"},
	{L"Asia/Yekaterinburg", L"Ekaterinburg Standard Time"},
	{L"Asia/Karachi", L"Pakistan Standard Time"},
	{L"Asia/Qyzylorda", L"Qyzylorda Standard Time"},
	{L"Asia/Calcutta", L"India Standard Time"},
	{L"Asia/Kolkata", L"India Standard Time"},
	{L"Asia/Colombo", L"Sri Lanka Standard Time"},
	{L"Asia/Katmandu", L"Nepal Standard Time"},
	{L"Asia/Kathmandu", L"Nepal Standard Time"},
	{L"Asia/Bishkek", L"Central Asia Standard Time"},
	{L"Antarctica/Vostok", L"Central Asia Standard Time"},
	{L"Asia/Urumqi", L"Central Asia Standard Time"},
	{L"Indian/Chagos", L"Central Asia Standard Time"},
	{L"Asia/Dhaka", L"Bangladesh Standard Time"},
	{L"Asia/Thimphu", L"Bangladesh Standard Time"},
	{L"Asia/Omsk", L"Omsk Standard Time"},
	{L"Asia/Rangoon", L"Myanmar Standard Time"},
	{L"Asia/Yangon", L"Myanmar Standard Time"},
	{L"Indian/Cocos", L"Myanmar Standard Time"},
	{L"Asia/Bangkok", L"SE Asia Standard Time"},
	{L"Antarctica/Davis", L"SE Asia Standard Time"},
	{L"Indian/Christmas", L"SE Asia Standard Time"},
	{L"Asia/Jakarta", L"SE Asia Standard Time"},
	{L"Asia/Pontianak", L"SE Asia Standard Time"},
	{L"Asia/Phnom_Penh", L"SE Asia Standard Time"},
	{L"Asia/Vientiane", L"SE Asia Standard Time"},
	{L"Asia/Saigon", L"SE Asia Standard Time"},
	{L"Asia/Ho_Chi_Minh", L"SE Asia Standard Time"},
	{L"Asia/Barnaul", L"Altai Standard Time"},
	{L"Asia/Hovd", L"W. Mongolia Standard Time"},
	{L"Asia/Krasnoyarsk", L"North Asia Standard Time"},
	{L"Asia/Novokuznetsk", L"North Asia Standard Time"},
	{L"Asia/Novosibirsk", L"N. Central Asia Standard Time"},
	{L"Asia/Tomsk", L"Tomsk Standard Time"},
	{L"Asia/Shanghai", L"China Standard Time"},
	{L"Asia/Hong_Kong", L"China Standard Time"},
	{L"Asia/Macau", L"China Standard Time"},
	{L"Asia/Irkutsk", L"North Asia East Standard Time"},
	{L"Asia/Singapore", L"Singapore Standard Time"},
	{L"Asia/Brunei", L"Singapore Standard Time"},
	{L"Asia/Makassar", L"Singapore Standard Time"},
	{L"Asia/Kuala_Lumpur", L"Singapore Standard Time"},
	{L"Asia/Kuching", L"Singapore Standard Time"},
	{L"Asia/Manila", L"Singapore Standard Time"},
	{L"Australia/Perth", L"W. Australia Standard Time"},
	{L"Asia/Taipei", L"Taipei Standard Time"},
	{L"Asia/Ulaanbaatar", L"Ulaanbaatar Standard Time"},
	{L"Australia/Eucla", L"Aus Central W. Standard Time"},
	{L"Asia/Chita", L"Transbaikal Standard Time"},
	{L"Asia/Tokyo", L"Tokyo Standard Time"},
	{L"Asia/Jayapura", L"Tokyo Standard Time"},
	{L"Pacific/Palau", L"Tokyo Standard Time"},
	{L"Asia/Dili", L"Tokyo Standard Time"},
	{L"Asia/Pyongyang", L"North Korea Standard Time"},
	{L"Asia/Seoul", L"Korea Standard Time"},
	{L"Asia/Yakutsk", L"Yakutsk Standard Time"},
	{L"Asia/Khandyga", L"Yakutsk Standard Time"},
	{L"Australia/Adelaide", L"Cen. Australia Standard Time"},
	{L"Australia/Broken_Hill", L"Cen. Australia Standard Time"},
	{L"Australia/Darwin", L"AUS Central Standard Time"},
	{L"Australia/Brisbane", L"E. Australia Standard Time"},
	{L"Australia/Lindeman", L"E. Australia Standard Time"},
	{L"Australia/Sydney", L"AUS Eastern Standard Time"},
	{L"Australia/Melbourne", L"AUS Eastern Standard Time"},
	{L"Pacific/Port_Moresby", L"West Pacific Standard Time"},
	{L"Antarctica/DumontDUrville", L"West Pacific Standard Time"},
	{L"Pacific/Truk", L"West Pacific Standard Time"},
	{L"Pacific/Guam", L"West Pacific Standard Time"},
	{L"Pacific/Saipan", L"West Pacific Standard Time"},
	{L"Australia/Hobart", L"Tasmania Standard Time"},
	{L"Antarctica/Macquarie", L"Tasmania Standard Time"},
	{L"Asia/Vladivostok", L"Vladivostok Standard Time"},
	{L"Asia/Ust-Nera", L"Vladivostok Standard Time"},
	{L"Australia/Lord_Howe", L"Lord Howe Standard Time"},
	{L"Pacific/Bougainville", L"Bougainville Standard Time"},
	{L"Asia/Srednekolymsk", L"Russia Time Zone 10"},
	{L"Asia/Magadan", L"Magadan Standard Time"},
	{L"Pacific/Norfolk", L"Norfolk Standard Time"},
	{L"Asia/Sakhalin", L"Sakhalin Standard Time"},
	{L"Pacific/Guadalcanal", L"Central Pacific Standard Time"},
	{L"Antarctica/Casey", L"Central Pacific Standard Time"},
	{L"Pacific/Ponape", L"Central Pacific Standard Time"},
	{L"Pacific/Kosrae", L"Central Pacific Standard Time"},
	{L"Pacific/Noumea", L"Central Pacific Standard Time"},
	{L"Pacific/Efate", L"Central Pacific Standard Time"},
	{L"Asia/Kamchatka", L"Russia Time Zone 11"},
	{L"Asia/Anadyr", L"Russia Time Zone 11"},
	{L"Pacific/Auckland", L"New Zealand Standard Time"},
	{L"Antarctica/McMurdo", L"New Zealand Standard Time"},
	{L"Etc/GMT-12", L"UTC+12"},
	{L"Pacific/Tarawa", L"UTC+12"},
	{L"Pacific/Majuro", L"UTC+12"},
	{L"Pacific/Kwajalein", L"UTC+12"},
	{L"Pacific/Nauru", L"UTC+12"},
	{L"Pacific/Funafuti", L"UTC+12"},
	{L"Pacific/Wake", L"UTC+12"},
	{L"Pacific/Wallis", L"UTC+12"},
	{L"Pacific/Fiji", L"Fiji Standard Time"},
	{L"Pacific/Chatham", L"Chatham Islands Standard Time"},
	{L"Etc/GMT-13", L"UTC+13"},
	{L"Pacific/Enderbury", L"UTC+13"},
	{L"Pacific/Fakaofo", L"UTC+13"},
	{L"Pacific/Tongatapu", L"Tonga Standard Time"},
	{L"Pacific/Apia", L"Samoa Standard Time"},
	{L"Pacific/Kiritimati", L"Line Islands Standard Time"},
};

static const FORMAT_ZONE* FindFormatZone(const wchar_t* name, size_t length)
{
	size_t i;
	for(i=0; i<_countof(m_format_zones); ++i) {
		if(wcslen(m_format_zones[i].iana) == length &&
			!wcsncmp(m_format_zones[i].iana, name, length))
			return &m_format_zones[i];
	}
	return NULL;
}

static int ConvertFormatZone(const SYSTEMTIME* utc, const FORMAT_ZONE* zone,
	SYSTEMTIME* local)
{
	TIME_ZONE_INFORMATION tzi;
	FORMAT_TZI raw_tzi;
	HKEY key = NULL;
	DWORD size = sizeof(raw_tzi), type;
	wchar_t path[256];
	if(!wcscmp(zone->windows, L"UTC")) {
		*local = *utc;
		return 1;
	}
	ZeroMemory(&tzi, sizeof(tzi));
	wsprintf(path, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\%s", zone->windows);
	if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &key) != ERROR_SUCCESS ||
		RegQueryValueExW(key, L"TZI", NULL, &type, (BYTE*)&raw_tzi, &size) != ERROR_SUCCESS ||
		type != REG_BINARY) {
		if(key) RegCloseKey(key);
		return 0;
	}
	RegCloseKey(key);
	tzi.Bias = raw_tzi.bias;
	tzi.StandardBias = raw_tzi.standard_bias;
	tzi.DaylightBias = raw_tzi.daylight_bias;
	tzi.StandardDate = raw_tzi.standard_date;
	tzi.DaylightDate = raw_tzi.daylight_date;
	if(!SystemTimeToTzSpecificLocalTime(&tzi, utc, local))
		return 0;
	return 1;
}

static void WriteFormatOffset(wchar_t** out, int offset)
{
	int hours, minutes;
	wchar_t sign = '+';
	if(offset < 0) { sign = '-'; offset = -offset; }
	hours = offset / 60;
	minutes = offset % 60;
	*(*out)++ = sign;
	*(*out)++ = (wchar_t)('0' + hours / 10);
	*(*out)++ = (wchar_t)('0' + hours % 10);
	*(*out)++ = ':';
	*(*out)++ = (wchar_t)('0' + minutes / 10);
	*(*out)++ = (wchar_t)('0' + minutes % 10);
}

static int GetFormatOffset(const SYSTEMTIME* utc, const SYSTEMTIME* local)
{
	FILETIME utc_file, local_file;
	ULARGE_INTEGER utc_value, local_value;
	if(!SystemTimeToFileTime(utc, &utc_file) || !SystemTimeToFileTime(local, &local_file))
		return 0;
	utc_value.LowPart = utc_file.dwLowDateTime;
	utc_value.HighPart = utc_file.dwHighDateTime;
	local_value.LowPart = local_file.dwLowDateTime;
	local_value.HighPart = local_file.dwHighDateTime;
	return (int)((LONGLONG)(local_value.QuadPart - utc_value.QuadPart) / 600000000);
}

extern char g_bHourZero;

//================================================================================================
//---------------------------------//+++--> load Localized Strings for Month, Day, & AM/PM Symbols:
void InitFormat(const wchar_t* section, SYSTEMTIME* lt)   //--------------------------------------------------------+++-->
{
	wchar_t year[6];
	int i, ilang, ioptcal;
	
	ilang = api.GetInt(section, L"Locale", GetUserDefaultLangID());
	
	GetLocaleInfo(ilang, LOCALE_IDEFAULTANSICODEPAGE|LOCALE_RETURN_NUMBER, (wchar_t*)&m_codepage, sizeof(m_codepage));
	if(!IsValidCodePage(m_codepage)) m_codepage=CP_ACP;
	
	i = lt->wDayOfWeek - 1;
	if(i < 0) i = 6;
	
	GetLocaleInfo(ilang, LOCALE_SABBREVDAYNAME1 + i, m_DayOfWeekShort, _countof(m_DayOfWeekShort));
//	GetLocaleInfo(ilang, LOCALE_SSHORTESTDAYNAME1 + i, DayOfWeekShort, _countof(DayOfWeekShort)); // Vista+
	GetLocaleInfo(ilang, LOCALE_SDAYNAME1 + i, m_DayOfWeekLong, _countof(m_DayOfWeekLong));
	
	i = lt->wMonth; i--;
	GetLocaleInfo(ilang, LOCALE_SABBREVMONTHNAME1 + i, m_MonthShort, _countof(m_MonthShort));
	GetLocaleInfo(ilang, LOCALE_SMONTHNAME1 + i, m_MonthLong, _countof(m_MonthLong));
	
	api.GetStr(section, L"AMsymbol", m_AM, _countof(m_AM), L"");
	if(!*m_AM){
		GetLocaleInfo(ilang, LOCALE_S1159, m_AM, _countof(m_AM));
		if(!m_AM[0]) wcscpy(m_AM, L"AM");
	}
	api.GetStr(section, L"PMsymbol", m_PM, _countof(m_PM), L"");
	if(!*m_PM){
		GetLocaleInfo(ilang, LOCALE_S2359, m_PM, _countof(m_PM));
		if(!m_PM[0]) wcscpy(m_PM, L"PM");
	}
	
	m_AltYear = -1;
	
	if(!GetLocaleInfo(ilang, LOCALE_IOPTIONALCALENDAR|LOCALE_RETURN_NUMBER, (wchar_t*)&ioptcal, sizeof(ioptcal)))
		ioptcal = 0;
	
	if(ioptcal < 3) ilang = LANG_USER_DEFAULT;
	
	if(!GetDateFormat(ilang, DATE_USE_ALT_CALENDAR, lt, L"gg", m_EraStr, _countof(m_EraStr)))
		*m_EraStr = '\0';
	
	if(GetDateFormat(ilang, DATE_USE_ALT_CALENDAR, lt, L"yyyy", year, _countof(year)))
		m_AltYear = _wtoi(year);
}

__pragma(warning(push))
__pragma(warning(disable:4701)) // MSVC is confused with our S(..) format (uptime) about "num" being "uninitialized"
//================================================================================================
//-------------+++--> Format T-Clock's OutPut String From Current Date, Time, & System Information:
unsigned MakeFormat(wchar_t buf[FORMAT_MAX_SIZE], const wchar_t* fmt, SYSTEMTIME* pt, int beat100)   //------------------+++-->
{
	const wchar_t* bufend = buf+FORMAT_MAX_SIZE;
	const wchar_t* pos;
	wchar_t* out = buf;
	SYSTEMTIME utc, zonetime;
	int zone_offset = 0;
	const FORMAT_ZONE* zone;
	ULONGLONG TickCount = 0;
	if(!TzSpecificLocalTimeToSystemTime(NULL, pt, &utc))
		utc = *pt;
	
	while(*fmt) {
		if(*fmt == '[') {
			const wchar_t* zone_start = ++fmt;
			while(*fmt && *fmt != ']') ++fmt;
			zone = FindFormatZone(zone_start, (size_t)(fmt-zone_start));
			if(zone) {
				if(ConvertFormatZone(&utc, zone, &zonetime)) {
					pt = &zonetime;
					zone_offset = GetFormatOffset(&utc, &zonetime);
				}
			} else {
				while(zone_start < fmt) *out++ = *zone_start++;
			}
			if(*fmt) ++fmt;
			continue;
		}
		if(*fmt == '"') {
			wchar_t end = '"';
			for(++fmt; *fmt&&*fmt!=end; )
				*out++ = *fmt++;
			if(*fmt == end) ++fmt;
			continue;
		}
		if(*fmt=='\\' && fmt[1]=='n') {
			fmt+=2;
			*out++='\n';
		}
		/// for testing
		else if(*fmt == 'S' && fmt[1] == 'S' && (fmt[2] == 'S' || fmt[2] == 's')) {
			fmt += 3;
			out += api.WriteFormatNum(out, (int)pt->wSecond, 2, 0);
			*out++ = '.';
			out += api.WriteFormatNum(out, (int)pt->wMilliseconds, 3, 0);
		}
		
		else if(*fmt == 'y' && fmt[1] == 'y') {
			int len;
			len = 2;
			if(*(fmt + 2) == 'y' && *(fmt + 3) == 'y') len = 4;
			
			out += api.WriteFormatNum(out, (len==2)?(int)pt->wYear%100:(int)pt->wYear, len, 0);
			fmt += len;
		} else if(*fmt == 'm') {
			if(*(fmt + 1) == 'm' && *(fmt + 2) == 'e') {
				*out++ = m_MonthEng[pt->wMonth-1][0];
				*out++ = m_MonthEng[pt->wMonth-1][1];
				*out++ = m_MonthEng[pt->wMonth-1][2];
				fmt += 3;
			} else if(fmt[1] == 'm' && fmt[2] == 'm') {
				if(*(fmt + 3) == 'm') {
					fmt += 4;
					for(pos=m_MonthLong; *pos; ) *out++=*pos++;
				} else {
					fmt += 3;
					for(pos=m_MonthShort; *pos; ) *out++=*pos++;
				}
			} else {
				if(fmt[1] == 'm') {
					fmt += 2;
					*out++ = (wchar_t)((int)pt->wMonth / 10) + '0';
				} else {
					++fmt;
					if(pt->wMonth > 9)
						*out++ = (wchar_t)((int)pt->wMonth / 10) + '0';
				}
				*out++ = (wchar_t)((int)pt->wMonth % 10) + '0';
			}
		} else if(*fmt == 'a' && fmt[1] == 'a' && fmt[2] == 'a') {
			fmt += 3;
			if(*fmt == 'a') {
				++fmt;
				pos = m_DayOfWeekLong;
			} else {
				pos = m_DayOfWeekShort;
			}
			for(; *pos; *out++ = *pos++);
		} else if(*fmt=='d') {
			if(fmt[1]=='d' && fmt[2]=='e'){
				fmt+=3;
				for(pos=m_DayOfWeekEng[pt->wDayOfWeek]; *pos; ) *out++=*pos++;
			}else if(fmt[1]=='d' && fmt[2]=='d') {
				fmt += 3;
				if(*fmt == 'd'){
					++fmt;
					pos = m_DayOfWeekLong;
				}else{
					pos = m_DayOfWeekShort;
				}
				for(; *pos; *out++ = *pos++);
			}else{
				if(fmt[1]=='d') {
					fmt+=2;
					*out++ = (wchar_t)((int)pt->wDay / 10) + '0';
				}else{
					++fmt;
					if(pt->wDay > 9)
						*out++ = (wchar_t)((int)pt->wDay / 10) + '0';
				}
				*out++ = (wchar_t)((int)pt->wDay % 10) + '0';
			}
		} else if(*fmt=='h') {
			int hour = pt->wHour;
			while(hour >= 12) // faster than mod 12 if "hour" <= 24
				hour -= 12;
			if(!hour && !g_bHourZero)
				hour = 12;
			if(fmt[1] == 'h') {
				fmt += 2;
				*out++ = (wchar_t)(hour / 10) + '0';
			} else {
				++fmt;
				if(hour > 9)
					*out++ = (wchar_t)(hour / 10) + '0';
			}
			*out++ = (wchar_t)(hour % 10) + '0';
		} else if(*fmt=='H') {
			if(fmt[1] == 'H') {
				fmt += 2;
				*out++ = (wchar_t)(pt->wHour / 10) + '0';
			} else {
				++fmt;
				if(pt->wHour > 9)
					*out++ = (wchar_t)(pt->wHour / 10) + '0';
			}
			*out++ = (wchar_t)(pt->wHour % 10) + '0';
		} else if((*fmt=='w'||*fmt=='W') && (fmt[1]=='+'||fmt[1]=='-')) {
			char is_12h = (*fmt == 'w');
			char is_negative = (*++fmt == '-');
			int hour = 0;
			for(; *++fmt<='9'&&*fmt>='0'; ){
				hour *= 10;
				hour += *fmt-'0';
			}
			if(is_negative) hour = -hour;
			hour = (pt->wHour + hour)%24;
			if(hour < 0) hour += 24;
			if(is_12h){
				while(hour >= 12) // faster than mod 12 if "hour" <= 24
					hour -= 12;
				if(!hour && !g_bHourZero)
					hour = 12;
			}
			*out++ = (wchar_t)(hour / 10) + '0';
			*out++ = (wchar_t)(hour % 10) + '0';
		} else if(*fmt == 'n') {
			if(fmt[1] == 'n') {
				fmt += 2;
				*out++ = (wchar_t)((int)pt->wMinute / 10) + '0';
			} else {
				++fmt;
				if(pt->wMinute > 9)
					*out++ = (wchar_t)((int)pt->wMinute / 10) + '0';
			}
			*out++ = (wchar_t)((int)pt->wMinute % 10) + '0';
		} else if(*fmt == 's') {
			if(fmt[1] == 's') {
				fmt += 2;
				*out++ = (wchar_t)((int)pt->wSecond / 10) + '0';
			} else {
				++fmt;
				if(pt->wSecond > 9)
					*out++ = (wchar_t)((int)pt->wSecond / 10) + '0';
			}
			*out++ = (wchar_t)((int)pt->wSecond % 10) + '0';
		} else if(*fmt == 't' && fmt[1] == 't') {
			fmt += 2;
			if(pt->wHour < 12) pos = m_AM; else pos = m_PM;
			while(*pos) *out++ = *pos++;
		} else if(*fmt == 'A' && fmt[1] == 'M') {
			if(fmt[2] == '/' &&
			   fmt[3] == 'P' && fmt[4] == 'M') {
				if(pt->wHour < 12) *out++ = 'A'; //--+++--> 2010 - Noon / MidNight Decided Here!
				else *out++ = 'P';
				*out++ = 'M'; fmt += 5;
			} else if(fmt[2] == 'P' && fmt[3] == 'M') {
				fmt += 4;
				if(pt->wHour < 12) pos = m_AM; else pos = m_PM;
				while(*pos) *out++ = *pos++;
			} else *out++ = *fmt++;
		} else if(*fmt == 'a' && fmt[1] == 'm' && fmt[2] == '/' &&
				  fmt[3] == 'p' && fmt[4] == 'm') {
			if(pt->wHour < 12) *out++ = 'a';
			else *out++ = 'p';
			*out++ = 'm'; fmt += 5;
		}
		// internet time
		else if(*fmt == '@' && fmt[1] == '@' && fmt[2] == '@') {
			fmt += 3;
			*out++ = '@';
			*out++ = (wchar_t)(beat100 / 10000) + '0';
			*out++ = (wchar_t)((beat100 % 10000) / 1000) + '0';
			*out++ = (wchar_t)((beat100 % 1000) / 100) + '0';
			if(*fmt=='.' && fmt[1]=='@') {
				fmt += 2;
				*out++ = '.';
				*out++ = (wchar_t)((beat100 % 100) / 10) + '0';
				if(*fmt=='@'){
					++fmt;
					*out++ = (wchar_t)((beat100 % 10)) + '0';
				}
			}
		}
		// alternate calendar
		else if(*fmt == 'Y' && m_AltYear > -1) {
			int n = 1;
			while(*fmt == 'Y') { n *= 10; ++fmt; }
			if(n < m_AltYear) {
				n = 1; while(n < m_AltYear) n *= 10;
			}
			for(;;) {
				*out++ = (wchar_t)((m_AltYear % n) / (n/10)) + '0';
				if(n == 10) break;
				n /= 10;
			}
		} else if(*fmt == 'g') {
			for(pos=m_EraStr; *pos&&*fmt=='g'; ){
				*out++ = *pos++;
				++fmt;
			}
			while(*fmt == 'g') fmt++;
		}
		
		else if(*fmt == 'L' && wcsncmp(fmt, L"LDATE", 5) == 0) {
			GetDateFormat(LOCALE_USER_DEFAULT,
						  DATE_LONGDATE, pt, NULL, out, (int)(bufend-out));
			for(; *out; ++out);
			fmt += 5;
		}
		
		else if(*fmt == 'D' && wcsncmp(fmt, L"DATE", 4) == 0) {
			GetDateFormat(LOCALE_USER_DEFAULT,
						  DATE_SHORTDATE, pt, NULL, out, (int)(bufend-out));
			for(; *out; ++out);
			fmt += 4;
		}
		
		else if(*fmt == 'T' && wcsncmp(fmt, L"TIME", 4) == 0) {
			GetTimeFormat(LOCALE_USER_DEFAULT,
						  0, pt, NULL, out, (int)(bufend-out));
			for(; *out; ++out);
			fmt += 4;
		} else if(*fmt == 'Z' && fmt[1] == 'Z') {
			fmt += 2;
			WriteFormatOffset(&out, zone_offset);
		} else if(*fmt == 'S') { // uptime
			int width, padding, num;
			const wchar_t* old_fmt = ++fmt;
			wchar_t specifier = api.GetFormat(&fmt, &width, &padding);
			if(!TickCount) TickCount = api.GetTickCount64();
			switch(specifier){
			case 'd'://days
				num = (int)(TickCount/86400000);
				break;
			case 'a'://hours total
				num = (int)(TickCount/3600000);
				break;
			case 'h'://hours (max 24)
				num = (TickCount/3600000)%24;
				break;
			case 'n'://minutes
				num = (TickCount/60000)%60;
				break;
			case 's'://seconds
				num = (TickCount/1000)%60;
				break;
			case 'T':{// ST, uptime as h:mm:ss
				ULONGLONG past = TickCount/1000;
				int hour, minute;
				num = past%60; past /= 60;
				minute = past%60; past /= 60;
				hour = (int)past;
				
				out += api.WriteFormatNum(out, hour, width, padding);
				*out++ = ':';
				out += api.WriteFormatNum(out, minute, 2, 0);
				*out++ = ':';
				width = 2; padding = 0;
				break;}
			default:
				specifier = '\0';
				fmt = old_fmt;
				*out++ = 'S';
			}
			if(specifier)
				out += api.WriteFormatNum(out, num, width, padding);
		} else if(fmt[0] == 'w') { // numeric Day-of-Week
			int weekday = pt->wDayOfWeek;
			++fmt;
			if(*fmt == 'i' || *fmt == 'u') {
				if(!weekday && *fmt == 'i')
					weekday = 7;
				++fmt;
				*out++ = '0' + (char)weekday;
			} else {
				*out++ = 'w';
			}
		} else if(*fmt == 'W') { // Week-of-Year
			char buf[4];
			int width, padding, num;
			wchar_t specifier;
			struct tm tmnow;
			time_t ts = time(NULL);
			localtime_r(&ts, &tmnow);
			api.GetFormat(&fmt, &width, &padding);
			specifier = *fmt;
			switch(specifier){
			case 's': // Week-Of-Year Starts Sunday
				strftime(buf, _countof(buf), "%U", &tmnow);
				num = atoi(buf);
				break;
			case 'm': // Week-Of-Year Starts Monday
				strftime(buf, _countof(buf), "%W", &tmnow);
				num = atoi(buf);
				break;
			case 'i':{ // ISO-8601 week (1st version by henriko.se, 2nd by White-Tiger)
				int wday, borderdays, week;
				for(;;){
					wday = (!tmnow.tm_wday ? 6 : tmnow.tm_wday - 1); // convert from Sun-Sat to Mon-Sun (0-5)
					borderdays = (1 + tmnow.tm_yday + 6 - wday) % 7;
					week = (1 + tmnow.tm_yday + 6 - wday) / 7;
					if(borderdays >= 4){ // year starts with at least 4 days
						++week;
					} else if(!week){ // we're still in last year's week
						--tmnow.tm_year;
						tmnow.tm_mon = 11;
						tmnow.tm_mday = 31;
						tmnow.tm_isdst = -1;
						if(mktime(&tmnow)==-1){ // mktime magically updates tm_yday, tm_wday
							week = 1;
							break; // fail safe
						}
						tmnow.tm_mon = 0; // just to speed up the "if" below, since we know that it can't be week 1
						continue; // repeat (once)
					}
					if(tmnow.tm_mon==11 && tmnow.tm_mday>=29){ // end of year, could be week 1
						borderdays = 31 - tmnow.tm_mday + wday;
						if(borderdays < 3)
							week = 1;
					}
					break;
				}
				num = week;
				break;}
			case 'u': // U.S. like Week-of-Year - beginning on first day of the year, based on sunday
				num = (1 + (tmnow.tm_yday + 6 - tmnow.tm_wday) / 7);
				break;
			case 'w': // SWN (Simple Week Number)
				num = (1 + tmnow.tm_yday / 7);
				break;
			default:
				specifier = '\0';
				*out++ = 'W';
			}
			if(specifier) {
				++fmt;
				out += api.WriteFormatNum(out, num, width, padding);
			}
		}
//================================================================================================
//======================================= JULIAN DATE Code ========================================
		else if(*fmt == 'J' && *(fmt + 1) == 'D') {
			double y, M, d, h, m, s, bc, JD;
			struct tm Julian;
			int id, is, i;
			char* szJulian;
			time_t UTC = time(NULL);
			
			gmtime_r(&UTC, &Julian);
			
			y = Julian.tm_year +1900;	// Year
			M = Julian.tm_mon +1;		// Month
			d = Julian.tm_mday;			// Day
			h = Julian.tm_hour;			// Hours
			m = Julian.tm_min;			// Minutes
			s = Julian.tm_sec;			// Seconds
			// This Handles the January 1, 4713 B.C up to
			bc = 100.0 * y + M - 190002.5; // Year 0 Part.
			JD = 367.0 * y;
			
			JD -= floor(7.0*(y + floor((M+9.0)/12.0))/4.0);
			JD += floor(275.0*M/9.0);
			JD += d;
			JD += (h + (m + s/60.0)/60.0)/24.0;
			JD += 1721013.5; // BCE 2 November 18 00:00:00.0 UT - Tuesday
			JD -= 0.5*bc/fabs(bc);
			JD += 0.5;
			
			szJulian = _fcvt(JD, 4, &id, &is); // Make it a String
			for(i=0; szJulian[0]; ++i) {
				if(i == id) { //--//-++-> id = Decimal Point Precision/Position
					*out++ = '.'; // ReInsert the Decimal Point Where it Belongs.
				} else {
					*out++ = *szJulian++; //--+++--> Done!
				}
			}
			fmt +=2;
		}
//================================================================================================
//======================================= ORDINAL DATE Code =======================================
		else if(*fmt == 'O' && *(fmt + 1) == 'D') { //--------+++--> Ordinal Date UTC:
			struct tm today;
			time_t UTC = time(NULL);
			
			gmtime_r(&UTC, &today);
			out += wcsftime(out, 16, L"%Y-%j", &today);
			fmt +=2;
		}
		//==========================================================================
		else if(*fmt == 'O' && *(fmt + 1) == 'd') { //------+++--> Ordinal Date Local:
			struct tm today;
			time_t ts = time(NULL);
			
			localtime_r(&ts, &today);
			out += wcsftime(out, 16, L"%Y-%j", &today);
			fmt +=2;
		}
		//==========================================================================
		else if(*fmt == 'D' && wcsncmp(fmt, L"DOY", 3) == 0) { //--+++--> Day-Of-Year:
			struct tm today;
			time_t ts = time(NULL);
			
			localtime_r(&ts, &today);
			out += wcsftime(out, 8, L"%j", &today);
			fmt +=3;
		}
		//==========================================================================
		else if(*fmt == 'P' && wcsncmp(fmt, L"POSIX", 5) == 0) { //-> Posix/Unix Time:
			out += wsprintf(out, FMT("%") FMT(PRIi64), (int64_t)time(NULL));
			fmt +=5;
		}
		//==========================================================================
		else if(*fmt == 'T' && wcsncmp(fmt, L"TZN", 3) == 0) { //--++-> TimeZone Name:
			#ifndef __GNUC__ /* forces us to link with msvcr100 */
			char szTZName[TZNAME_MAX] = {0};
			size_t lRet;
			char* tzn;
			int iDST;
			
			_get_daylight(&iDST);
			if(iDST) {
				_get_tzname(&lRet, szTZName, _countof(szTZName), 1);
			} else {
				_get_tzname(&lRet, szTZName, _countof(szTZName), 0);
			}
			
			tzn = szTZName;
			while(*tzn) *out++ = *tzn++;
			#endif
			fmt +=3;
		}
//=================================================================================================
		else {
			*out++ = *fmt++;
		}
	}
	*out='\0';
	return (unsigned)(out-buf);
}
__pragma(warning(pop))

/*--------------------------------------------------
--------------------------------------- Check Format
--------------------------------------------------*/
DWORD FindFormat(const wchar_t* fmt)
{
	DWORD ret = 0;
	
	while(*fmt) {
		if(*fmt == '"' || *fmt == '[') {
			wchar_t end = (*fmt == '[') ? ']' : '"';
			do{
				for(++fmt; *fmt&&*fmt++!=end; );
			}while(*fmt == '"' || *fmt == '[');
			if(!*fmt)
				break;
		}
		
		else if(*fmt == 's') {
			fmt++;
			ret |= FORMAT_SECOND;
		}
		else if(*fmt == 'T' && wcsncmp(fmt, L"TIME", 4) == 0) {
			fmt += 4;
			ret |= FORMAT_SECOND;
		}
		
		else if(*fmt == '@' && fmt[1] == '@' && fmt[2] == '@') {
			fmt += 3;
			if(*fmt == '.' && fmt[1] == '@') {
				ret |= FORMAT_BEAT2;
				fmt += 2;
			} else ret |= FORMAT_BEAT1;
		}
		
		else ++fmt;
	}
	return ret;
}
