#include "moarvm.h"

/* This representation's function pointer table. */
static MVMREPROps *this_repr;

/* Creates a new type object of this representation, and associates it with
 * the given HOW. */
static MVMObject * type_object_for(MVMThreadContext *tc, MVMObject *HOW) {
    MVMSTable *st  = MVM_gc_allocate_stable(tc, this_repr, HOW);
    MVMObject *obj = MVM_gc_allocate_type_object(tc, st);
    st->WHAT = obj;
    return st->WHAT;
}

/* Creates a new instance based on the type object. */
static MVMObject * allocate(MVMThreadContext *tc, MVMSTable *st) {
    return MVM_gc_allocate_object(tc, st, sizeof(MVMArray));
}

/* Initialize a new instance. */
static void initialize(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data) {
}

/* Copies to the body of one object to another. */
static void copy_to(MVMThreadContext *tc, MVMSTable *st, void *src, MVMObject *dest_root, void *dest) {
    MVMArrayBody *src_body  = (MVMArrayBody *)src;
    MVMArrayBody *dest_body = (MVMArrayBody *)dest;
    dest_body->elems = src_body->elems;
    dest_body->alloc  = src_body->alloc;
    dest_body->data   = malloc(sizeof(MVMObject *) * dest_body->alloc);
    memcpy(dest_body->data, src_body->data, sizeof(MVMObject *) * dest_body->elems);
}

/* Called by the VM in order to free memory associated with this object. */
static void gc_free(MVMThreadContext *tc, MVMObject *obj) {
    MVMArray *str = (MVMArray *)obj;
    free(str->body.data);
    str->body.data = NULL;
    str->body.elems = str->body.alloc = 0;
}

/* Gets the storage specification for this representation. */
static MVMStorageSpec get_storage_spec(MVMThreadContext *tc, MVMSTable *st) {
    MVMStorageSpec spec;
    spec.inlineable      = MVM_STORAGE_SPEC_REFERENCE;
    spec.boxed_primitive = MVM_STORAGE_SPEC_BP_NONE;
    spec.can_box         = 0;
    return spec;
}

/* Initializes the representation. */
MVMREPROps * MVMArray_initialize(MVMThreadContext *tc) {
    /* Allocate and populate the representation function table. Note
     * that to support the bootstrap, this one REPR guards against a
     * duplicate initialization (which we actually will do). */
    if (!this_repr) {
        this_repr = malloc(sizeof(MVMREPROps));
        memset(this_repr, 0, sizeof(MVMREPROps));
        this_repr->type_object_for = type_object_for;
        this_repr->allocate = allocate;
        this_repr->initialize = initialize;
        this_repr->copy_to = copy_to;
        this_repr->gc_free = gc_free;
        this_repr->get_storage_spec = get_storage_spec;
    }
    return this_repr;
}