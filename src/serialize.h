/*
 This file declares all non-member ::serialize and ::unserialize
 functions for classes from external libraries, such as STL containers
 and Eigen matrices. The template functions defined here allow the
 serialization of most STL containers used in the rest of the code,
 even ones that are nested to arbitrary depths.
 
 Any class you define that needs to be serialized should inherit from
 the pure abstract class "serializable", defined in serializable.h.
*/

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include "serializable.h"

template <typename U, typename V> void serialize(const std::pair<U, V> &p, std::ostream &os);
template <typename U, typename V> void unserialize(std::pair<U, V> &p, std::istream &is);

template <typename T> void serialize(const T* v, std::ostream &os);
template <typename T> void unserialize(T* &v, std::istream &is);

template <typename C> void serialize_container(const C &container, std::ostream &os);

template <typename T> void serialize(const std::vector<T> &v, std::ostream &os);
template <typename T> void unserialize(std::vector<T> &v, std::istream &is);

template <typename T> void serialize(const std::set<T> &s, std::ostream &os);
template <typename T> void unserialize(std::set<T> &s, std::istream &is);

template <typename K, typename V> void serialize(const std::map<K, V> &m, std::ostream &os);
template <typename K, typename V> void unserialize(std::map<K, V> &m, std::istream &is);


inline void serialize(const serializable &p, std::ostream &os) {
	p.serialize(os);
}

inline void unserialize(serializable &p, std::istream &is) {
	p.unserialize(is);
}

inline void serialize(int i, std::ostream &os) {
	os << i << std::endl;
}

inline void unserialize(int &i, std::istream &is) {
	if (!(is >> i)) {
		assert(false);
	}
}

inline void serialize(bool b, std::ostream &os) {
	os << (b ? 't' : 'f') << std::endl;
}

inline void unserialize(bool &b, std::istream &is) {
	char c;
	if (!(is >> c)) {
		assert(false);
	}
	assert(c == 't' || c == 'f');
	b = (c == 't');
}

/*
 It's impossible to get an exact decimal representation of floating point
 numbers, so I use hexadecimal floating point representation instead. This
 gives up on readability but prevents rounding errors in the
 serialize/unserialize cycle.
*/
inline void serialize(double v, std::ostream &os) {
	enum { BUFSIZE = 40 };
	static char buf[BUFSIZE];
	
	if (std::snprintf(buf, BUFSIZE, "%a", v) == BUFSIZE) {
		std::cerr << "buffer overflow when serializing a double" << std::endl;
		assert(false);
	}
	os << buf << std::endl;
}

/*
 The stream operator >> doesn't recognize hex float format, so use strtod
 instead.
*/
inline void unserialize(double &v, std::istream &is) {
	static std::string buf;
	char *end;
	
	if (!(is >> buf)) {
		assert(false);
	}
	v = std::strtod(buf.c_str(), &end);
	if (*end != '\0') {
		std::cerr << "Unrecognized float format (" << buf << ") when unserializing double" << std::endl;
		assert(false);
	}
}

inline void serialize(const std::string &s, std::ostream &os) {
	os << s.size() << std::endl;
	os << s << std::endl;
}

inline void unserialize(std::string &s, std::istream &is) {
	int n;
	is >> n;
	s.resize(n);
	if (is.get() != '\n') {
		assert(false);
	}
	for (int i = 0; i < n; ++i) {
		if (!is.get(s[i])) {
			assert(false);
		}
	}
}

template <typename U, typename V>
void serialize(const std::pair<U, V> &p, std::ostream &os) {
	serialize(p.first, os);
	serialize(p.second, os);
}

template <typename U, typename V>
void unserialize(std::pair<U, V> &p, std::istream &is) {
	unserialize(p.first, is);
	unserialize(p.second, is);
}

template <typename T>
void serialize(T const *v, std::ostream &os) {
	if (!v) {
		os << "0" << std::endl;
	} else {
		os << "1" << std::endl;
		serialize(*v, os);
	}
}

template <typename T>
void unserialize(T *&v, std::istream &is) {
	char c;
	is >> c;
	if (c == '0') {
		v = NULL;
	} else {
		assert(c == '1');
		T *t = new T;
		unserialize(*t, is);
		v = t;
	}
}

template <typename T>
void unserialize(T const *&v, std::istream &is) {
	T *&u = const_cast<T*&>(v);
	unserialize(u, is);
}

template <typename C>
void serialize_container(const C &container, std::ostream &os) {
	os << container.size() << " [" << std::endl;
	typename C::const_iterator i;
	for (i = container.begin(); i != container.end(); ++i) {
		serialize(*i, os);
	}
	os << ']' << std::endl;
}

template <typename T>
void serialize(const std::vector<T> &v, std::ostream &os) {
	serialize_container<std::vector<T> >(v, os);
}

template <typename T>
void unserialize(std::vector<T> &v, std::istream &is) {
	char bracket;
	int n = 0;
	if (!(is >> n >> bracket) || bracket != '[') {
		assert(false);
	}
	v.resize(n);
	for (int i = 0; i < n; ++i) {
		unserialize(v[i], is);
	}
	if (!(is >> bracket) || bracket != ']') {
		assert(false);
	}
}

template <typename T>
void serialize(const std::set<T> &s, std::ostream &os) {
	serialize_container<std::set<T> >(s, os);
}

template <typename T>
void unserialize(std::set<T> &s, std::istream &is) {
	char bracket;
	int n = 0;
	if (!(is >> n >> bracket) || bracket != '[') {
		assert(false);
	}
	s.clear();
	T elem;
	for (int i = 0; i < n; ++i) {
		unserialize(elem, is);
		s.insert(elem);
	}
	if (!(is >> bracket) || bracket != ']') {
		assert(false);
	}
	/*
	 If elem contains a pointer and doesn't have a custom assignment operator,
	 then at this point elem may be holding the same pointer as the last element
	 inserted into the set. That pointer may be deallocated by the destructor when
	 elem goes out of scope, which would result in an invalid pointer in the set.
	 This next line fixes the problem by setting the pointer to something else,
	 hopefully NULL.
	*/
	elem = T();
}

template <typename K, typename V>
void serialize(const std::map<K, V> &m, std::ostream &os) {
	serialize_container<std::map<K,V> >(m, os);
}

template <typename K, typename V>
void unserialize(std::map<K, V> &m, std::istream &is) {
	char bracket;
	int n = 0;
	if (!(is >> n >> bracket) || bracket != '[') {
		assert(false);
	}
	m.clear();
	K key;
	for (int j = 0; j < n; ++j) {
		unserialize(key, is);
		unserialize(m[key], is);
	}
	if (!(is >> bracket) || bracket != ']') {
		assert(false);
	}
	/*
	 Reset key for the same reason as in the set unserializer.
	*/
	key = K();
}

class serializer {
public:
	serializer(std::ostream &os) : os(os) {}

	template <typename T>
	serializer &operator<<(const T &obj) {
		::serialize(obj, os);
		return *this;
	}

private:
	std::ostream &os;
};

class unserializer {
public:
	unserializer(std::istream &is) : is(is) {}

	template <typename T>
	unserializer &operator>>(T &obj) {
		::unserialize(obj, is);
		return *this;
	}

private:
	std::istream &is;
};

#endif

