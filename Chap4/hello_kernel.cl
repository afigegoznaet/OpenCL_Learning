__kernel void hello_kernel(__global char16 *msg) {
#ifdef __ENDIAN_LITTLE__
   *msg = (char16)('H', 'e', 'l', 'l', 'o', ' ',
      'k', 'e', 'r', 'n', 'e', 'l', '!', '!', '!', '\0');
#else
   *msg = (char16)('B', 'i', 'g', ' ', 'e', 'n',
      'd', 'i', 'a', 'n', '\0', '\0', '\0', '\0', '\0', '\0');
#endif
}
