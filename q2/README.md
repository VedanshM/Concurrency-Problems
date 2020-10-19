# Back to College

## Build n Run

+ Build: `gcc -pthread q2.c`
+ Run: `./a.out`

## Basic structure

Each company, zone and student has its own thread.

## Data structures

### Comapany

Each `company_t` consists of:

+ Company id
+ no of batches prepared
+ array of batches prepared
+ succesc probability
+ mutex

### Batch

+ capacity
+ no of vaccines left

### Zone

+ Unique zone id
+ phase: a value from (WAITING_FOR_BATCH,SEARCHING_STUDS, VACCINATING_STUDS)
+ no of slots allotted.
+ no of slots filled.
+ is vaccination done.
+ pointer to batch under use
+ pointer to company that made batch
+ thread id
+ mutex

### Student

+ unique student id
+ no of trials done (not more than 3)
+ vaccination status
+ thread id

## Implementation

Pseudo code for company_thread:

```python
company_thread():
    while students_not_done:
        # generate random size and count
        generate_batches()

        distribute_batches()

        while not all_batches_empty():
            #busy waiting
```

Pseudo code for distribute_batches():

```python
distribute_batches():
    i=0
    while i< no_of_batches:
        for zone in all_zones:
            lock(zone.mutex)
            if not zone.batch:
                zone.batch = batches[i]
                i+=1
                zone.company = self
                unlock(zone.mutex)
                break
            unlock(mutex)
```

Pseudo code for zone_thread:

```python
zone_thread():
    while not all_students_done:
        #waiting for a company to send batch
        while 1:
            lock(self.mutex)
            if self.batch:
                unlock(self.mutex)
            unlock(self.mutex)

        #now we have a new batch
        while vaccine_left:
            lock(self.mutex)
            self.slots = random()
            unlock(self.mutex)

            #waiting for students to come
            while 1:
                lock(self.mutex)
                lock(global_mutex) #for students_waiting

                if self.slots.filled() or students_waiting = 0:
                    unlock(self.mutex)
                    unlock(global_mutex)
                    break
                else:
                    unlock(self.mutex)
                    unlock(global_mutex)

            #vaccination phase
            self.student_can_go=1

            #locking companies mutex to update batch status
            lock(self.company.mutex)
            self.batch.update()
            if self.batch.empty():
                unlock(self.company.mutex)
                self.batch = NULL
                # now we will again procure a new batch
                break
            else:
                unlock(self.company.mutex)

```

Pseudo code for student_thread():

```python
stud_thread():
    while self.no_of_trials < 3:
        self.no_of_trials+=1
        lock(global_mutex)
        students_waiting++
        unlock(global_mutex)

        get_vaccine()

        if vaccine_successful:
            lock(global_mutex)
            students_done+=1
            unlock(global_mutex)
            break
        else:
            if self.no_of_trials == 3:
                lock(global_mutex)
                students_done+=1
                unlock(global_mutex)
            else:
                continue

```
