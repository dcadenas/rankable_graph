#include <ruby.h>
#include <glib.h>

typedef struct {
  GPtrArray* in_links;
  GPtrArray* number_out_links;
  gint current_available_index;
  GHashTable* key_to_index;
  GHashTable* index_to_key;
} RNStruct;


static void rankable_graph_free (RNStruct *p){
  g_hash_table_destroy(p->index_to_key);
  g_hash_table_destroy(p->key_to_index);
  g_ptr_array_free(p->number_out_links, TRUE);
  g_ptr_array_free(p->in_links, TRUE);
}

static void array_of_arrays_free_func (gpointer array){
  g_array_free(array, TRUE);
}

static VALUE rankable_graph_allocate (VALUE klass){
  RNStruct* rn;
  rn = ALLOC(RNStruct);
  rn->in_links = g_ptr_array_new_with_free_func(array_of_arrays_free_func);
  rn->number_out_links = g_ptr_array_new_with_free_func(g_free);
  rn->key_to_index = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
  rn->index_to_key = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

  return Data_Wrap_Struct(klass, 0, rankable_graph_free, rn);
}

static VALUE init(VALUE self){
  RNStruct* rn;
  Data_Get_Struct(self, RNStruct, rn);
  rn->current_available_index = -1;
  return self;
}

static void fill_empty_holes_in_in_links(RNStruct *rn){
  GArray* in_links_array;
  const gint in_links_size = rn->in_links->len, size = g_hash_table_size(rn->key_to_index);
  gint i;
  if(in_links_size < size){
    for(i = 0; i < size - in_links_size; i++){ 
      in_links_array = g_array_new(FALSE, FALSE, sizeof(gint));
      g_ptr_array_add(rn->in_links, in_links_array);
    }
  }
}

static void fill_empty_holes_in_number_out_links(RNStruct *rn){
  gint i, out_links_size = rn->number_out_links->len;
  const gint size = g_hash_table_size(rn->key_to_index);
  gint* zero;
  if(out_links_size < size){
    for(i = 0; i < size - out_links_size; i++){ 
      zero = g_new(gint, 1);
      *zero = 0;
      g_ptr_array_add(rn->number_out_links, zero);
    }
  }
}

static void update_in_links(RNStruct *rn, gint from, gint to){
  fill_empty_holes_in_in_links(rn);
  GArray* in_links_for_to = g_ptr_array_index(rn->in_links, to);
  g_array_append_val(in_links_for_to, from);
}

static void update_number_out_links(RNStruct *rn, gint from){
  fill_empty_holes_in_number_out_links(rn);
  gint* current_value = (gint *)g_ptr_array_index(rn->number_out_links, from);
  *current_value = (*current_value)++;
}

static void link_with_indices(RNStruct *rn, gint from, gint to){
  update_in_links(rn, from, to);
  update_number_out_links(rn, from);
}

static gint key_as_array_index(RNStruct *rn, VALUE key){
  gpointer key_as_index_ptr;
  gint key_as_int = FIX2INT(key);

  if(!g_hash_table_lookup_extended(rn->key_to_index, GINT_TO_POINTER(key_as_int), NULL, &key_as_index_ptr)){
    key_as_index_ptr = GINT_TO_POINTER(++rn->current_available_index);
    g_hash_table_insert(rn->key_to_index, GINT_TO_POINTER(key_as_int), key_as_index_ptr);
    g_hash_table_insert(rn->index_to_key, key_as_index_ptr, GINT_TO_POINTER(key_as_int));
  }

  return GPOINTER_TO_INT(key_as_index_ptr);
}

static VALUE link(VALUE self, VALUE from, VALUE to){
  RNStruct* rn;
  Data_Get_Struct(self, RNStruct, rn);

  gint from_as_index = key_as_array_index(rn, from);
  gint to_as_index = key_as_array_index(rn, to);

  link_with_indices(rn, from_as_index, to_as_index);
  return Qnil;
}

static gfloat calculate_change(gfloat *a, gfloat *b, gint size){
  gint i;
  gfloat acc = 0;
  for(i = 0; i < size; i++){
    acc += ABS(a[i] - b[i]);
  }
  return acc;
}

static GArray * calculate_dangling_nodes(RNStruct *rn){
  GArray* dangling_nodes = g_array_new(FALSE, FALSE, sizeof(gint));
  gint i;
  gpointer int_as_pointer;
  for(i = 0; i < rn->number_out_links->len; i++){
    if(*(gint *)g_ptr_array_index(rn->number_out_links, i) == 0){
      int_as_pointer = GINT_TO_POINTER(i);
      g_array_append_val(dangling_nodes, int_as_pointer);
    }
  }
  return dangling_nodes;
}

static gfloat* step(gfloat s, gfloat t_over_size, gfloat *p, RNStruct *rn, GArray *dangling_nodes){
  const gint size = g_hash_table_size(rn->key_to_index);
  gint i, j;
  gfloat inner_product = 0;
  for(i = 0; i < dangling_nodes->len; i++){
    inner_product += p[GPOINTER_TO_INT(g_array_index(dangling_nodes, gint, i))];
  }
  const gfloat inner_product_over_size = inner_product / (gfloat)size;

  gfloat ksum, vsum = 0;
  gint index;
  gfloat* v = g_new0(gfloat, size);
  GArray* in_links_for_i;
  for(i = 0; i < size; i++){
    ksum = 0;
    in_links_for_i = (GArray *)g_ptr_array_index(rn->in_links, i);
    for(j = 0; j < in_links_for_i->len; j++){
      index = GPOINTER_TO_INT(g_array_index(in_links_for_i, gint, j));
      ksum += p[index] / *((gint *)g_ptr_array_index(rn->number_out_links, index));
    }

    v[i] = s * (ksum + inner_product_over_size) + t_over_size;
    vsum += v[i];
  }

  const gfloat inverse_of_vsum = 1 / vsum;
  for(i = 0; i < size; i++){
    v[i] *= inverse_of_vsum;
  }
  return v;
}

static VALUE rank(VALUE self, VALUE s, VALUE tolerance){
  if(rb_block_given_p() == Qtrue){
    RNStruct* rn;
    Data_Get_Struct(self, RNStruct, rn);

    const gint size = g_hash_table_size(rn->key_to_index);
    const gfloat inverse_of_size = 1.0 / size;
    const gfloat t_over_size = (1.0 - NUM2DBL(s)) / size;

    g_assert_cmpuint(rn->in_links->len, ==, size);
    g_assert_cmpuint(rn->number_out_links->len, ==, size);
    GArray* dangling_nodes = calculate_dangling_nodes(rn);
    gfloat* p = g_new(gfloat, size);
    gint i;
    for(i = 0; i < size; i++){
      p[i] = inverse_of_size;
    }

    gfloat* new_p;
    gfloat change = 2;
    while(change > NUM2DBL(tolerance)){
      new_p = step(NUM2DBL(s), t_over_size, p, rn, dangling_nodes);
      change = calculate_change(p, new_p, size);
      g_free(p);
      p = new_p;
    }

    for(i = 0; i < size; i++){
      rb_yield_values(2, INT2FIX(g_hash_table_lookup(rn->index_to_key, GINT_TO_POINTER(i))), rb_float_new(p[i]));
    }

    g_free(p);
    g_array_free(dangling_nodes, TRUE);
  }
  return Qnil;
}

// Copy across state (used by clone and dup)
static VALUE rankable_graph_init_copy(VALUE copy, VALUE orig){
  RNStruct* orig_rn;
  RNStruct* copy_rn;

  if (copy == orig) return copy;

  if (TYPE(orig) != T_DATA || RDATA(orig)->dfree != (RUBY_DATA_FUNC)rankable_graph_free) {
    rb_raise(rb_eTypeError, "wrong argument type");
  }

  Data_Get_Struct(orig, RNStruct, orig_rn);
  Data_Get_Struct(copy, RNStruct, copy_rn);
  MEMCPY(copy_rn, orig_rn, RNStruct, 1);
  return copy;
}

static VALUE clear(VALUE self){
  RNStruct* rn;
  Data_Get_Struct(self, RNStruct, rn);

  rn->current_available_index = -1;
  g_hash_table_remove_all(rn->index_to_key);
  g_hash_table_remove_all(rn->key_to_index);
  g_ptr_array_set_size(rn->number_out_links, 0);
  g_ptr_array_set_size(rn->in_links, 0);

  return Qnil;
}

static VALUE rb_cRankableGraph;

void Init_rankable_graph(){
  rb_cRankableGraph = rb_define_class("RankableGraph", rb_cObject);
  rb_define_alloc_func(rb_cRankableGraph, rankable_graph_allocate);
  rb_define_method(rb_cRankableGraph, "initialize", init, 0);
  rb_define_method(rb_cRankableGraph, "initialize_copy", rankable_graph_init_copy, 1);
  rb_define_method(rb_cRankableGraph, "link", link, 2);
  rb_define_method(rb_cRankableGraph, "clear", clear, 0);
  rb_define_method(rb_cRankableGraph, "rank", rank, 2);
}

