/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"
#include "lib/kernel/hash.h"
#include "threads/vaddr.h"
#include "userprog/process.h"

/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */
void
vm_init (void) {
	vm_anon_init ();
	vm_file_init ();
#ifdef EFILESYS  /* For project 4 */
	pagecache_init ();
#endif
	register_inspect_intr ();
	/* DO NOT MODIFY UPPER LINES. */
	/* TODO: Your code goes here. */
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type
page_get_type (struct page *page) {
	int ty = VM_TYPE (page->operations->type);
	switch (ty) {
		case VM_UNINIT:
			return VM_TYPE (page->uninit.type);
		default:
			return ty;
	}
}

/* Helpers */
static struct frame *vm_get_victim (void);
static bool vm_do_claim_page (struct page *page);
static struct frame *vm_evict_frame (void);

/* Create the pending page object with initializer. If you want to create a
 * page, do not create it directly and make it through this function or
 * `vm_alloc_page`. */
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {

	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;

	/* Check wheter the upage is already occupied or not. */
	if (spt_find_page (spt, upage) == NULL) {
		/* TODO: Create the page, fetch the initialier according to the VM type,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */

		/* TODO: Insert the page into the spt. */
	}
err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL. */
/* 인자로 넘겨진 spt에서 가상주소 VA와 대응되는 페이지 구조체 찾아서 반환 */
struct page *
spt_find_page (struct supplemental_page_table *spt UNUSED, void *va UNUSED) {
	struct page *page = NULL;
	/* TODO: Fill this function. */
	/* pg_round_down()으로 vaddr의 페이지 번호를 얻음 */
	page = malloc(sizeof(page));	// page 사이즈(byte 단위)만큼 메모리 할당. hash_elem 인자로 사용하기 위해 만든 dummy page!
	page->va = pg_round_down(va);	// va가 가리키는 페이지의 시작 주소 반환 (offset 0)

	/* hash_find() 함수를 사용해서 hash_elem 구조체 얻음 */
	struct hash_elem *e;	// hash_find()가 hash_elem에 대한 포인터 반환하기 때문에 e는 포인터로 선언되어야 함.
	e = hash_find (&spt->pages, &page->hash_elem);
	free(page);	// hash_elem 인자로 사용했으니까 dummy page 할당 해제

	/* hash_entry()로 해당 hash_elem의 vm_entry 구조체 리턴
	   만약 존재하지 않는다면 NULL 리턴 */
	if (!e)
		return NULL;
	else
		return e;
}

/* Insert PAGE into spt with validation. */
/* 인자로 주어진 spt에 페이지 구조체 삽입 */
bool
spt_insert_page (struct supplemental_page_table *spt UNUSED,
		struct page *page UNUSED) {
	/* TODO: Fill this function. */
	/* hash_insert() 함수를 이용하여 vm_entry를 해시 테이블에 삽입 */
	if (!hash_insert (&spt->pages, &page->hash_elem))	// 삽입 성공시 NULL 반환
		return true;
	else
		return false;
}

void
spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
	/* hash_delete() 함수를 이용하여 vm_entry를 해시 테이블에서 제거 */
	hash_delete (&spt->pages, &page->hash_elem)
	vm_dealloc_page (page);
	// return true;
}

/* Get the struct frame, that will be evicted. */
static struct frame *
vm_get_victim (void) {
	struct frame *victim = NULL;
	 /* TODO: The policy for eviction is up to you. */

	return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.*/
static struct frame *
vm_evict_frame (void) {
	struct frame *victim UNUSED = vm_get_victim ();
	/* TODO: swap out the victim and return the evicted frame. */

	return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/
/* 모든 유저 영역 페이지들은 이 함수를 통해 할당해야함. */
static struct frame *
vm_get_frame (void) {
	struct frame *frame = NULL;
	/* TODO: Fill this function. */
	void *kva = palloc_get_page(PAL_USER);	// 메모리 풀에서 새로운 물리 메모리 페이지를 가져옴

	if (kva == NULL)	// 페이지 할당 실패
		PANIC("todo");
	
	/* 유저 메모리 풀에서 페이지 성공적으로 가져오면 */
	frame = malloc(sizeof(struct frame));	// 구조체 프레임 크기에 맞게 메모리 할당
	// frame = malloc(sizeof(*frame));	// 이렇게 작성해도 됨. 
	// frame = malloc(sizeof(frame));	// 이건 포인터의 크기에 따라 메모리 할당되므로 X

	// 프레임 구조체 멤버 초기화
	frame->kva = kva;
	frame->page = NULL;

	ASSERT (frame != NULL);
	ASSERT (frame->page == NULL);
	return frame;	// 해당 프레임 반환
}

/* Growing the stack. */
static void
vm_stack_growth (void *addr UNUSED) {
}

/* Handle the fault on write_protected page */
static bool
vm_handle_wp (struct page *page UNUSED) {
}

/* Return true on success */
/* page fault 발생시 제어권 얻음 */
bool
vm_try_handle_fault (struct intr_frame *f UNUSED, void *addr UNUSED,
		bool user UNUSED, bool write UNUSED, bool not_present UNUSED) {
	struct supplemental_page_table *spt UNUSED = &thread_current ()->spt;
	struct page *page = NULL;
	/* TODO: Validate the fault */
	/* TODO: Your code goes here */
	// 유효한 page fault인지 검사
	// 유효하지 않은 페이지에 접근한 page fault라면? (매핑되지 않은 페이지에 접근했다면) -> page fault 맞다

	// 유효하지 않은 페이지에 접근한 page fault가 아니라면? (= bogus fault. 물리 메모리와 매핑은 되어있지만 콘텐츠 로드 안되어있다면)
	// 페이지에서 콘텐츠 로드하고 유저프로그램에게 제어권 반환
	// vm_alloc_page_with_initializer() 구현, 함수에서 세팅해 놓은 초기화 함수 호출, 이 함수를 인자로 갖는 uninit_new 함수 호출
	// lazy_load_segment() 구현

	return vm_do_claim_page (page);
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void
vm_dealloc_page (struct page *page) {
	destroy (page);
	free (page);
}

/* Claim the page that allocate on VA. */
/* va에 페이지 할당하고 해당 페이지에 프레임 할당 */
bool
vm_claim_page (void *va UNUSED) {
	struct page *page = NULL;	// 한 페이지 얻음
	/* TODO: Fill this function */
	page = spt_find_page(&thread_current()->spt, va);	// va와 대응되는 페이지 구조체 반환
	
	if (page == NULL)	// spt_find_page(): va에 대응하는 페이지 구조체가 존재하지 않으면 NULL 반환
		return false;
	return vm_do_claim_page (page);	// 해당 페이지를 인자로 갖는 vm_do_claim_page() 호출
}

/* Claim the PAGE and set up the mmu. */
/* page에 물리 메모리 프레임 할당 */
static bool
vm_do_claim_page (struct page *page) {
	struct frame *frame = vm_get_frame ();	// 프레임 하나 얻음

	/* Set links */
	frame->page = page;
	page->frame = frame;

	/* TODO: Insert page table entry to map page's VA to frame's PA. */
	/* MMU 세팅 (페이지 테이블에 가상 주소와 물리 주소 매핑한 정보 추가)*/
	if (install_page(page->va, frame->kva, page->writable))
		return swap_in (page, frame->kva);	// 연산 성공
	else
		return false;	// 연산 실패
}

/* Initialize new supplemental page table */
/* userprog/process.c의 initd 함수로 새로운 프로세스가 시작하거나
   process.c의 __do_fork로 자식 프로세스가 생성될 때 호출되는 함수 */
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	/* hash_init()으로 해시테이블 초기화
	   인자로 해시 테이블과 vm_hash_func과 vm_less_func 사용 */
	hash_init (&spt->pages, page_hash, page_less, NULL);
}

/* Copy supplemental page table from src to dst */
bool
supplemental_page_table_copy (struct supplemental_page_table *dst UNUSED,
		struct supplemental_page_table *src UNUSED) {
}

/* Free the resource hold by the supplemental page table */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */
}
