/**
  *
  * WMI
  * @author Thomas Sparber (2016)
  *
 **/

#ifndef WMI_HPP
#define WMI_HPP

#include <string>

#include <wmiexception.hpp>
#include <wmiresult.hpp>

namespace Wmi
{

	void query(const std::string &q, WmiResult &out);

	inline WmiResult query(const std::string &q)
	{
		WmiResult result;
		query(q, result);
		return result;
	}

	template <class WmiClass>
	inline void retrieveWmi(WmiClass &out)
	{
		WmiResult result;
		const std::string q = std::string("Select * From ") + WmiClass::getWmiClassName();
		query(q, result);
		out.setProperties(result, 0);
	}

	template <class WmiClass>
	inline WmiClass retrieveWmi()
	{
		WmiClass temp;
		retrieveWmi(temp);
		return temp;
	}

	template <class WmiClass>
	inline void retrieveAllWmi(std::vector<WmiClass> &out)
	{
		WmiResult result;
		const std::string q = std::string("Select * From ") + WmiClass::getWmiClassName();
		query(q, result);

		out.clear();
		for(std::size_t index = 0; index < result.size(); ++index)
		{
			WmiClass temp;
			temp.setProperties(result, index);
			out.push_back(std::move(temp));
		}
	}

	template <class WmiClass>
	inline std::vector<WmiClass> retrieveAllWmi()
	{
		std::vector<WmiClass> ret;
		retrieveAllWmi(ret);

		return ret;
	}

}; //end namespace Wmi

#endif //WMI_HPP