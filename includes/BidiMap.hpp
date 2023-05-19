#include <map>

/*
 * Bidirectional map with key and value access.
 * The KeyType and valueType must be different
 */
template <typename KeyType, typename ValueType> class BidiMap
{
  private:
	typedef std::map<KeyType, ValueType> _KVmap;
	typedef std::map<ValueType, KeyType> _VKmap;
	_KVmap _key_to_value_map;
	_VKmap _value_to_key_map;

  public:
	BidiMap()
	{
	}
	BidiMap(const _KVmap &map)
	{
		for (typename _KVmap::const_iterator it
			 = map.begin();
			 it != map.end(); it++)
		{
			_key_to_value_map[it->first] = it->second;
			_value_to_key_map[it->second] = it->first;
		}
	}
	BidiMap(const BidiMap &orig)
		: _key_to_value_map(orig._key_to_value_map),
		  _value_to_key_map(orig._value_to_key_map)
	{
	}
	BidiMap &operator=(const BidiMap &orig)
	{
		this->_key_to_value_map = orig._key_to_value_map;
		this->_value_to_key_map = orig._value_to_key_map;
		return (*this);
	}
	~BidiMap()
	{
	}

	void insert(const KeyType &key, const ValueType &value)
	{
		_key_to_value_map[key] = value;
		_value_to_key_map[value] = key;
	}
	ValueType &getValueByKey(const KeyType &key)
	{
		const typename _KVmap::iterator find
			= _key_to_value_map.find(key);

		if (find == _key_to_value_map.end())
			throw(std::runtime_error("Key not found"));
		return (find->second);
	}
	KeyType &getKeyByValue(const ValueType &value)
	{
		const typename _VKmap::iterator find
			= _value_to_key_map.find(value);

		if (find == _value_to_key_map.end())
			throw(std::runtime_error("Value not found"));
		return (find->second);
	}
	ValueType &operator[](const KeyType &key)
	{
		return (getValueByKey(key));
	}
	KeyType &operator[](const ValueType &value)
	{
		return (getKeyByValue(value));
	}
};
