#include<linux/module.h>
#include<linux/sched/signal.h>
#include<linux/mm.h>
#include<linux/pgtable.h>
#include<asm/io.h>

static unsigned long virt2phys(struct mm_struct *mm, unsigned long vpage);

int proc_init (void) {
  	printk(KERN_INFO "PROCESS REPORT:\n");
  	printk(KERN_INFO "proc_id, proc_name, contig_pages, noncontig_pages, total_pages\n");
  	struct task_struct *task;
  	struct vm_area_struct *vma = 0;
  	unsigned long vpage;
  	unsigned long previous = 0;
  	for_each_process(task) {
  		if (task->pid > 650) {
  			unsigned long contig_pages = 0;
  			unsigned long noncontig_pages = 0; 
  			if (task->mm && task->mm->mmap) {
        			for (vma = task->mm->mmap; vma; vma = vma->vm_next) {
          				for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) {
            					unsigned long physical_page_addr = virt2phys(task->mm, vpage);
            					if (physical_page_addr == previous + PAGE_SIZE) {
            						contig_pages++;
            					} else {
            						noncontig_pages++;
            					}
            					previous = physical_page_addr;					
            				}
            			}
            		}
            		printk(KERN_INFO "%d, %s, %ld, %ld, %ld\n", task->pid, task->comm, contig_pages, noncontig_pages, contig_pages + noncontig_pages);	
            	}
  	}
  	
	return 0;
}

static unsigned long virt2phys(struct mm_struct *mm, unsigned long vpage) {
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct page *page;
	unsigned long physical_page_addr;
	pgd = pgd_offset(mm, vpage);
	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
		return 0;
	}
	p4d = p4d_offset(pgd, vpage);
	if (p4d_none(*p4d) || p4d_bad(*p4d)) {
		return 0;
	}
	pud = pud_offset(p4d, vpage);
	if (pud_none(*pud) || pud_bad(*pud)) {
		return 0;
	}
	pmd = pmd_offset(pud, vpage);
	if (pmd_none(*pmd) || pmd_bad(*pmd)) {
		return 0;
	}
	if (!(pte = pte_offset_map(pmd, vpage))) {
		return 0;
	}
	if (!(page = pte_page(*pte))) {
		return 0;
	}
	physical_page_addr = page_to_phys(page);
	pte_unmap(pte);
	if (physical_page_addr==70368744173568) {
		return 0;
	}
	return physical_page_addr;
}


void proc_cleanup(void) {
  	printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

