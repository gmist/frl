#ifndef FRL_INT_TO_TYPE_H_
#define FRL_INT_TO_TYPE_H_

namespace frl
{
	//	Структура для отображения целочисленных констант в типы
	template < int TYPE >
	struct IntToType
	{
		enum { value = TYPE };
	};
}// namespace FatRat Library

#endif /* FRL_INT_TO_TYPE_H_ */
