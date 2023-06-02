#ifndef FT_SHARED_PTR_HPP
#define FT_SHARED_PTR_HPP

#include <cstddef>

namespace ft
{
template <typename T> class shared_ptr
{
  private:
	T *_ptr;
	size_t *_count;

  public:
	shared_ptr(T *ptr = NULL) : _ptr(ptr), _count(new size_t(1))
	{
		(*_count) = 1;
	}

	shared_ptr(const shared_ptr &orig) : _ptr(orig._ptr), _count(orig._count)
	{
		(*_count)++;
	}

	shared_ptr &operator=(const shared_ptr &orig)
	{
		if (this == &orig)
			return (*this);

		(*_count)--;
		if ((*_count) == 0)
		{
			delete _ptr;
			delete _count;
		}

		_ptr = orig._ptr;
		_count = orig._count;
		(*_count)++;
		return (*this);
	}

	~shared_ptr()
	{
		(*_count)--;
		if ((*_count) == 0)
		{
			delete _ptr;
			delete _count;
		}
	}

	T &operator*(void)
	{
		return (*_ptr);
	}

	T *operator->(void)
	{
		return (_ptr);
	}
};
} // namespace ft

#endif
