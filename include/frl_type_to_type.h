#ifndef frl_type_to_type_h_
#define frl_type_to_type_h_

namespace frl{

//	Структура для отображения одного типа в другой
template < typename TYPE >
struct TypeToType
{
	typedef TYPE OriginalType;
};

} // FatRat Library

#endif // frl_type_to_type_h_
