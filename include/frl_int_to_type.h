#ifndef frl_int_to_type_h_
#define frl_int_to_type_h_

namespace frl
{

//	Структура для отображения целочисленных констант в типы
template < int TYPE >
struct IntToType
{
	enum { value = TYPE };
};

}// FatRat Library

#endif // frl_int_to_type_h_
