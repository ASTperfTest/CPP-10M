#ifndef __F86374E6_713C_4600_85FB_903A5CDF7251_IMPORT_EXPORT_H__
#define __F86374E6_713C_4600_85FB_903A5CDF7251_IMPORT_EXPORT_H__

INT_PTR CurrencyRates_Export(WPARAM wp, LPARAM lp);
INT_PTR CurrencyRates_Import(WPARAM wp, LPARAM lp);

using TNameValue = std::pair<const char*, const char*> ; // first is name,second is value
TNameValue parse_setting_node(const TiXmlNode *pXmlSetting);

#endif //__F86374E6_713C_4600_85FB_903A5CDF7251_IMPORT_EXPORT_H__
