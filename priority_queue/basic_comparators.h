#pragma once

template<class T>
struct less
{
	bool operator()(T, T);
};

template<class T>
struct greater
{
	bool operator()(T, T);
};

template<class T>
struct less_or_equal
{
	bool operator()(T, T);
};


template<class T>
struct greater_or_equal
{
	bool operator()(T, T);
};

