#pragma once
#include <stack>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <crtdbg.h>

using namespace std;

template<typename T>
class JWObjectPool
{
private:
	stack<shared_ptr<T>> _pool_list;
	size_t _block_capacity;

public:
	JWObjectPool(size_t block_capacity)
	{
		if (block_capacity <= 0)
		{
			printf_s("메모리 블록의 수용량은 양수이어야만 합니다\n");
			return;
		}

		_block_capacity = block_capacity;

		for (int i = 0; i < _block_capacity; ++i)
		{
			_pool_list.push(make_shared<T>());
		}
	}

	~JWObjectPool()
	{

	}

	shared_ptr<T> GetObject()
	{
		if (_pool_list.empty())
		{
			for (int i = 0; i < _block_capacity; ++i)
			{
				_pool_list.push(make_shared<T>());
			}
		}

		auto return_object = _pool_list.top();
		_pool_list.pop();

		printf("오브젝트를 풀에서 가져옵니다...\n");

		return return_object;
	}

	void ReleaseObject(shared_ptr<T> object)
	{
		_pool_list.push(object);

		printf("오브젝트를 풀에 릴리즈합니다...\n");
	}
};


class CharacterData 
{
public:
	CharacterData() {}

private :
	int _level = 1;
	float _hp = 100.f;
	char _type = 'A';
};

int main() 
{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		JWObjectPool<CharacterData> pool(100);

		// 플레이 세션 도중..
		auto char_tmp1 = pool.GetObject();
		auto char_tmp2 = pool.GetObject();
		auto char_tmp3 = pool.GetObject();
		auto char_tmp4 = pool.GetObject();
		auto char_tmp5 = pool.GetObject();

		pool.ReleaseObject(char_tmp1);
		pool.ReleaseObject(char_tmp2);
		pool.ReleaseObject(char_tmp3);
		pool.ReleaseObject(char_tmp4);
	//	pool.ReleaseObject(char_tmp5);



	return 0;
}