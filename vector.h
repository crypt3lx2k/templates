#ifndef TEMPLATES_VECTOR_H__
#define TEMPLATES_VECTOR_H__

#include <stddef.h>
#include <stdlib.h>

#ifndef VECTOR_RESIZE_FACTOR
# define VECTOR_RESIZE_FACTOR 0.70
#endif

#ifndef VECTOR_ERROR
# define VECTOR_ERROR NULL
#endif

/**
 * VECTOR_EMPTY_INITIALIZER
 * Provides a valid constant sized empty
 * vector of any type.
 */
#define VECTOR_EMPTY_INITIALIZER                \
  {NULL, NULL, NULL}

/**
 * vector(type)
 * Generic vector that provides
 * - O(1)
 *   - append (amortized)
 *   - capacity
 *   - clear
 *   - get
 *   - get_safe
 *   - is_empty
 *   - pop
 *   - set
 *   - size
 *   - trim
 * - O(n)
 *   - contains
 *   - index_of
 *   - insert
 *   - pop_first
 *   - prepend
 *   - remove
 *   - resize
 * operations.
 */
#define vector(type)                            \
  struct {                                      \
    type * base;                                \
    type * head;                                \
    type * end;                                 \
  }

/* returns the new size based
   on VECTOR_SIZE_REFACTOR and
   the old size*/
#define vector_newsize__(oldsize)               \
  ((size_t) ((VECTOR_RESIZE_FACTOR + 1) *       \
             (oldsize)) + 1)

/**
 * vector_append
 * Adds element to the end of vector.
 *
 * @param  (vector(type) *) vector in question
 * @param  (type) element to be added
 * @return (type) element that was added
 */
#define vector_append(v, e)                     \
  ({                                            \
    __typeof__(v) v2__ = (v);                   \
    __typeof__(*v2__->base) e__ = (e);          \
    __typeof__(v2__) b__ = v2__;                \
                                                \
    if (v2__->head == v2__->end) {              \
      ptrdiff_t cap__;                          \
      cap__ = (v2__->end - v2__->base);         \
                                                \
      b__ = vector_resize(                      \
                  v2__,                         \
                  vector_newsize__(cap__)       \
            );                                  \
    }                                           \
                                                \
    b__ == NULL ?                               \
      (__typeof__(*v2__->base)) VECTOR_ERROR :  \
      (*v2__->head++ = e__);                    \
  })

/**
 * vector_capacity
 * Returns the number of elements the current
 * allocated underlying array may hold.
 *
 * @param  (const vector(type) *) vector in question
 * @return (ptrdiff_t) capacity of vector
 */
#define vector_capacity(v)                      \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
                                                \
    v__->end - v__->base;                       \
  })

/**
 * vector_clear
 * Removes every element from vector
 * and frees the underlying array.
 *
 * @param  (vector(type) *) vector to clear
 * @return (vector(type) *) the cleared vector
 */
#define vector_clear(v)                         \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
                                                \
    free(v__->base);                            \
    v__->base = NULL;                           \
    v__->head = NULL;                           \
    v__->end  = NULL;                           \
                                                \
    v__;                                        \
  })

/**
 * vector_contains
 * Returns whether vector contains element.
 *
 * @param  (const vector(type) *) vector in question
 * @param  (type) element to check for existence
 * @param  (comparator-fn) comparator function
 * @return (int) non-zero if element was found in vector and zero otherwise
 */
#define vector_contains(v, e, eq)               \
  (vector_index_of(v, e, eq) != -1)

/**
 * vector_get
 * Returns the element at a certain
 * position in the vector without
 * removing it.
 *
 * @param  (const vector(type) *) the vector in question
 * @param  (int) index of element in vector
 * @return (type) the element at index in vector
 */
#define vector_get(v, i)                        \
  ((v)->base[(i)])

/**
 * vector_get_safe
 * Returns the element at a certain
 * position in the vector without
 * removing it.
 *
 * This returns VECTOR_ERROR casted
 * to type upon error.
 *
 * @param  (const vector(type) *) the vector in question
 * @param  (int) index of element in vector
 * @return (type) the element at index in vector
 */
#define vector_get_safe(v, i)                   \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
    int i__ = (i);                              \
                                                \
    ((i__ >= 0) &&                              \
     (i__ < vector_size(v))                     \
     ) ?                                        \
      v__->base[i__] :                          \
      (__typeof__(*v__->base)) VECTOR_ERROR;    \
  })

/**
 * vector_index_of
 * Returns the index of the first occurance of element in vector.
 *
 * @param  (const vector(type) *) vector in question
 * @param  (type) element to find index of
 * @param  (comparator-fn) comparator function
 * @return (int) index of element in vector or -1 otherwise
 */
#define vector_index_of(v, e, eq)               \
  ({                                            \
    __typeof__(v) v1__ = (v);                   \
    __typeof__(*v1__->base) e__ = (e);          \
    int i__;                                    \
    ptrdiff_t s__ = vector_size(v1__);          \
                                                \
    for (i__ = 0; i__ < s__; i__++)             \
      if (eq(e__, v1__->base[i__]) == 0)        \
        break;                                  \
                                                \
    i__ == s__ ? -1 : i__;                      \
  })

/**
 * vector_insert
 * Inserts an element at a certain position
 * in vector without removing any elements.
 *
 * @param  (vector(type) *) the vector in question
 * @param  (int) the index of the new element
 * @param  (type) the element to be added
 * @return (type) the added elemenet
 */
#define vector_insert(v, i, e)                      \
  ({                                                \
    __typeof__(v) v1__ = (v);                       \
    __typeof__(*v1__->base) e1__ = (e);             \
    __typeof__(*v1__->base) l__;                    \
    int i__ = (i);                                  \
    ptrdiff_t size__ = vector_size(v1__);           \
                                                    \
    l__ = (__typeof__(*v1__->base)) VECTOR_ERROR;   \
                                                    \
    if (i__ < 0)                                    \
      l__ = l__;                                    \
    else if (i__ == size__)                         \
      l__ = vector_append(v1__, e1__);              \
    else if (i__ < size__) {                        \
      __typeof__(*v1__->base) p__;                  \
      int j__;                                      \
                                                    \
      p__ = v1__->base[size__-1];                   \
                                                    \
      for (j__ = size__-1; j__ > i__; j__--)        \
        v1__->base[j__] = v1__->base[j__-1];        \
                                                    \
      v1__->base[i__] = e1__;                       \
                                                    \
      l__ = vector_append(v1__, p__) ==             \
        (__typeof__(*v1__->base)) VECTOR_ERROR ?    \
        l__ :                                       \
        e1__;                                       \
    } else                                          \
      l__ = l__;                                    \
                                                    \
    l__;                                            \
  })

/**
 * vector_is_empty
 * Returns whether the vector is empty.
 *
 * @param  (const vector(type) *) vector in question
 * @return (int) zero if the vector isn't empty and non-zero otherwise
 */
#define vector_is_empty(v)                      \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
                                                \
    v__->base == v__->head;                     \
  })

/**
 * vector_pop
 * Removes and returns the element
 * at the end of the vector.
 *
 * @param  (vector(type) *) vector in question
 * @return (type) the removed element
 */
#define vector_pop(v)                           \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
                                                \
    v__->head == v__->base ?                    \
      (__typeof__(*v__->base)) VECTOR_ERROR :   \
      *--v__->head;                             \
  })

/**
 * vector_pop_first
 * Removes and returns the first
 * element of the vector.
 *
 * @param  (vector(type) *) vector in question
 * @return (type) the removed element
 */
#define vector_pop_first(v)                     \
  ({                                            \
    __typeof__(v) v1__ = (v);                   \
    __typeof__(*v1__->base) l__;                \
    ptrdiff_t size__;                           \
                                                \
    size__ = vector_size(v1__);                 \
                                                \
    if (size__ > 1) {                           \
      int i__;                                  \
                                                \
      l__ = v1__->base[0];                      \
                                                \
      for (i__ = 0; i__ < size__-1; i__++)      \
        v1__->base[i__] = v1__->base[i__+1];    \
                                                \
      v1__->head -= 1;                          \
    } else {                                    \
      l__ = vector_pop(v1__);                   \
    }                                           \
                                                \
    l__;                                        \
  })

/**
 * vector_prepend
 * Adds element to the front of vector.
 *
 * @param  (vector(type) *) vector in question
 * @param  (type) element to be added to vector
 * @return (type) element that was added
 */
#define vector_prepend(v, e)                    \
  ({                                            \
    __typeof__(v) v1__ = (v);                   \
    __typeof__(*v1__->base) e__ = (e);          \
    __typeof__(*v1__->base) l__;                \
    ptrdiff_t size__ = vector_size(v1__);       \
                                                \
    if (size__ > 0) {                           \
      int i__;                                  \
                                                \
      l__ = v1__->base[size__-1];               \
                                                \
      for (i__ = size__-1; i__ > 0; i__--)      \
        v1__->base[i__] = v1__->base[i__-1];    \
                                                \
      v1__->base[0] = e__;                      \
    } else {                                    \
      l__ = e__;                                \
    }                                           \
                                                \
    vector_append(v1__, l__) ==                 \
      (__typeof__(*v1__->base)) VECTOR_ERROR ?  \
      (__typeof__(*v1__->base)) VECTOR_ERROR :  \
      e__;                                      \
  })

/**
 * vector_remove
 * Removes and returns an element at a
 * certain position in vector.
 *
 * @param  (vector(type) *) vector in question
 * @param  (int) index of element
 * @return  (type) the removed element
 */
#define vector_remove(v, i)                       \
  ({                                              \
    __typeof__(v) v1__ = (v);                     \
    __typeof__(*v1__->base) l__;                  \
    int i__ = (i);                                \
    ptrdiff_t size__ = vector_size(v1__);         \
                                                  \
    l__ = (__typeof__(*v1__->base)) VECTOR_ERROR; \
                                                  \
    if (i__ < 0)                                  \
      l__ = l__;                                  \
    else if (i__ == size__-1)                     \
      l__ = vector_pop(v1__);                     \
    else if (i__ < size__) {                      \
      int j__;                                    \
                                                  \
      l__ = v1__->base[i__];                      \
                                                  \
      for (j__ = i__; j__ < size__-1; j__++)      \
        v1__->base[j__] = v1__->base[j__+1];      \
                                                  \
      v1__->head -= 1;                            \
    } else {                                      \
      l__ = l__;                                  \
    }                                             \
                                                  \
    l__;                                          \
  })

/**
 * vector_resize
 * Resizes the underlying array of vector.
 * Note that resizes larger than vector_capacity
 * might result in the entire vector being copied.
 *
 * @param  (vector(type) *) the vector to resize
 * @param  (size_t) number of elements the new vector may hold
 * @return (vector(type) *) the resized vector
 */
#define vector_resize(v, s)                     \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
    __typeof__(v__->base) b__;                  \
    size_t s__ = (s);                           \
                                                \
    if (s__ != (size_t)                         \
        (v__->end - v__->base)) {               \
      ptrdiff_t o__ = v__->head - v__->base;    \
                                                \
      b__ = realloc(v__->base,                  \
                    s__ * sizeof(*v__->base));  \
                                                \
      if (b__ != NULL) {                        \
        v__->base = b__;                        \
        v__->head = b__ + o__;                  \
        v__->end  = b__ + s__;                  \
      }                                         \
    } else {                                    \
      b__ = v__->base;                          \
    }                                           \
                                                \
    b__ == NULL ? NULL : v__;                   \
  })

/**
 * vector_set
 * Sets the element at a certain position
 * in the vector and removes the old element.
 *
 * @param  (vector(type) *) the vector in question
 * @param  (int) index of new element
 * @param  (type) element to be added
 * @return (type) element that was added
 */
#define vector_set(v, i, e)                         \
  ({                                                \
    __typeof__(v) v1__ = (v);                       \
    __typeof__(*v1__->base) e1__ = (e);             \
    __typeof__(*v1__->base) l__;                    \
    int i__ = (i);                                  \
    ptrdiff_t size__;                               \
                                                    \
    size__ = vector_size(v);                        \
    l__ = (__typeof__(*v1__->base)) VECTOR_ERROR;   \
                                                    \
    if (i__ < 0)                                    \
      l__ = l__;                                    \
    else if (i__ < size__)                          \
      l__ = (v1__->base[i__] = e1__);               \
    else if (i__ == size__)                         \
      l__ = vector_append(v1__, e1__);              \
    else                                            \
      l__ = l__;                                    \
                                                    \
    l__;                                            \
  })

/**
 * vector_size
 * Returns the current number of elements
 * in the vector.
 *
 * @param  (const vector(type) *) vector in question
 * @return (ptrdiff_t) number of elements in vector
 */
#define vector_size(v)                          \
  ({                                            \
    __typeof__(v) v__ = (v);                    \
                                                \
    v__->head - v__->base;                      \
  })

/**
 * vector_trim
 * Resizes the underlying array so only the
 * elements currently in vector fits.
 *
 * @param  (vector(type) *) the vector in question
 * @return (vector(type) *) the trimmed vector
 */
#define vector_trim(v)                                  \
  ({                                                    \
    __typeof__(v) v1__ = (v);                           \
    __typeof__(v1__) b__ = v1__;                        \
                                                        \
    b__ = vector_resize(v1__, vector_size(v1__));       \
                                                        \
    b__ == NULL ? NULL :                                \
      v1__;                                             \
  })

#endif /* TEMPLATES_VECTOR_H__ */
