#ifndef FRL_TYPE_TO_TYPE_H_
#define FRL_TYPE_TO_TYPE_H_

namespace frl
{
	//	Структура для отображения одного типа в другой
	template < typename TYPE >
	struct TypeToType
	{
		typedef TYPE OriginalType;
	};
} // namespace FatRat Library

#endif /* FRL_TYPE_TO_TYPE_H_ */
