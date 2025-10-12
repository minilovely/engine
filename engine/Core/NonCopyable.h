//按照C++98的写法是所有继承NonCopyable的类都禁止复制，
//但是现代C++可以直接在类中禁止复制，不必要使用继承实现
#pragma once
class NonCopyable
{
public:
	NonCopyable();
	~NonCopyable();
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
private:

};

NonCopyable::NonCopyable()
{
}

NonCopyable::~NonCopyable()
{
}