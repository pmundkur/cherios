void	namespace_init(void *ns_ref, void *ns_id);
int	namespace_register(int nb, void *ref, void *id);
void *	namespace_get_ref(int nb);
void *	namespace_get_id(int nb);
int	namespace_get_num_services(void);

extern void * namespace_ref;
extern void * namespace_id;
