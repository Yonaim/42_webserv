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
	typedef typename _KVmap::iterator iterator;
	typedef typename _KVmap::const_iterator const_iterator;

	BidiMap()
	{
	}
	BidiMap(const std::pair<KeyType, ValueType> arr[], size_t n)
	{
		for (size_t i = 0; i < n; i++)
		{
			_key_to_value_map[arr[i].first] = arr[i].second;
			_value_to_key_map[arr[i].second] = arr[i].first;	
		}
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
	iterator begin(void)
	{
		return (_key_to_value_map.begin());
	}
	const_iterator begin(void) const
	{
		return (_key_to_value_map.begin());
	}
	iterator end(void)
	{
		return (_key_to_value_map.end());
	}
	const_iterator end(void) const
	{
		return (_key_to_value_map.end());
	}
	iterator find(const KeyType &key)
	{
		return (_key_to_value_map.find(key));
	}
	const_iterator find(const KeyType &key) const
	{
		return (_key_to_value_map.find(key));
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
	const ValueType &getValueByKey(const KeyType &key) const
	{
		const typename _KVmap::const_iterator find
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
	const KeyType &getKeyByValue(const ValueType &value) const
	{
		const typename _VKmap::const_iterator find
			= _value_to_key_map.find(value);

		if (find == _value_to_key_map.end())
			throw(std::runtime_error("Value not found"));
		return (find->second);
	}
	ValueType &operator[](const KeyType &key)
	{
		return (getValueByKey(key));
	}
	const ValueType &operator[](const KeyType &key) const
	{
		return (getValueByKey(key));
	}
	KeyType &operator[](const ValueType &value)
	{
		return (getKeyByValue(value));
	}
	const KeyType &operator[](const ValueType &value) const
	{
		return (getKeyByValue(value));
	}
};
