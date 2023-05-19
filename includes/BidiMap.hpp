#include <map>

/*
 * Bidirectional map with key and value access.
 * The KeyType and valueType must be different
 */
template <typename KeyType, typename ValueType> class BidiMap
{
  private:
	std::map<KeyType, ValueType> _key_to_value_map;
	std::map<ValueType, KeyType> _value_to_key_map;

  public:
	BidiMap()
	{
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
		const typename std::map<KeyType, ValueType>::iterator find
			= _key_to_value_map.find(key);

		if (find == _key_to_value_map.end())
			throw(std::runtime_error("Key not found"));
		return (find->second);
	}
	KeyType &getKeyByValue(const ValueType &value)
	{
		const typename std::map<ValueType, KeyType>::iterator find
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
