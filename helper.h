static inline uint32_t zigzag_encode32(int32_t n) {
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 31);
}

static inline int32_t zigzag_decode32(uint32_t n) {
  return (n >> 1) ^ - (int32_t)(n & 1);
}

static inline uint64_t zigzag_encode64(int64_t n) {
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 63);
}

static inline int64_t zigzag_decode64(uint64_t n) {
  return (n >> 1) ^ - (int64_t)(n & 1);
}

//static void pb_execute_wakeup(zval *obj TSRMLS_DC)
//{
//    zval fname, *retval_ptr = NULL;
//
//    if (Z_OBJCE_P(obj) != PHP_IC_ENTRY &&
//        zend_hash_exists(&Z_OBJCE_P(obj)->function_table, "__wakeup", sizeof("__wakeup"))) {
//
//            INIT_PZVAL(&fname);
//            ZVAL_STRINGL(&fname, "__wakeup", sizeof("__wakeup") -1, 0);
//
//            call_user_function_ex(CG(function_table), &obj, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);
//    }
//
//    if (retval_ptr) {
//        zval_ptr_dtor(&retval_ptr);
//    }
//}
//
//static void pb_execute_sleep(zval *obj, zval **retval TSRMLS_DC)
//{
//    zval fname, *retval_ptr = NULL;
//
//    if (Z_OBJCE_P(obj) != PHP_IC_ENTRY &&
//        zend_hash_exists(&Z_OBJCE_P(obj)->function_table, "__sleep", sizeof("__sleep"))) {
//
//            INIT_PZVAL(&fname);
//            ZVAL_STRINGL(&fname, "__sleep", sizeof("__sleep") -1, 0);
//
//            call_user_function_ex(CG(function_table), &obj, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);
//    }
//
//    if (retval_ptr) {
//        *retval = retval_ptr;
//    }
//}
