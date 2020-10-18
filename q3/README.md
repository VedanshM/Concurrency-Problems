# Musical Mayhem

## Build n Run

+ Build: `gcc -pthread q3.c`
+ Run: `./a.out`

## Basic structure

We have total `k` musicians each has his/her own thread. Singers are always dealt differently in most functions due to them being a special case. There are total 4 semaphores used that are:  

+ `ac_sem`: it's value corresponds to number of acoustic stages available.  
+ `ec_sem`: it's value corresponds to number of electric stages available.  
+ `both_sem`: it's value corresponds to number of any type of stages available. Essentially its value will always be sum of `ac_sem` and `ec_sem`.  
+ `singer_sem`: it's value corresponds to number of stages available for an singer.  
+ `coord_sem`: it's value corresponds to number of coordinators available. A musician thread will wait for it and pick t-shirt.

  > *Assumption*: A musician will not join a solo singer. Hence when a singer sings solo, it need to also lock the semaphores of that stage (eg `ac_sem` for acoustic) and hence `both_sem` to prevent any musician to come in.  

## Stage

To allow tracking of stages we have two arrays each for acoustic and electric stage.  
Each stage stores:

+ a unique stage id
+ stage type (acoustic or electric)
+ pointer to musician if performing
+ pointer to singer if performing
+ endtime of performance if going on
+ mutex to protect from simultaneous acquisiton

## Musician

The data structre `musician_t` store:

+ Name
+ Instrument Type
+ Stage type eligible
+ Arrrival time
+ Thread_id

Pseudo code for musician_thread:

```python
    musician_thread():
        sleep_till_arrival

        # based on 4 cases (singer, acoustic_only, electric_only, any_stage) perform timed wait on corresponding semaphore.
        sem_timed_wait()
        if timedout:
            #just leaving impatient, thread dies
            return  
        else:
            #acquire any additional semaphores
            #like if acquired ac_sem , acquire both_sem too

            # the function will search a stage of said type
            # and will return on performance completion
            # a -1 is returned if all stages busy
            perform_on_type(self, stage_type)

            #if all stages of type 1 are busy, then we must be eligible for type2  of stages too and there will guaranteed a free stage for us

            unlock_all_semaphores()

            #wait for coordinator
            sem_lock(coord_sem)

            sleep(2)

            sem_unlock(coord_sem)
            # leave happy, thread dies
            return
```

Pseudo code for perform_on_type:

```python
    perform_on_type():
        if singer:
            # find a stage by iterating over array of stages of given type
            stage.singer_performing = self
            if not stage.muscian_performing:
                #means solo performance
                #acquire stage semaphores to block muscian from entering
                acquire_locks()
            else:
                # shared performance inc stage's endtime
                # this will convey to musician too the new performance time
                stage.endtime+= 2

            sleep_till_performance()
            release_locks_if_acquired()
            return stage.index
        else:
            #means we are not singer

            # find a stage by iterating over array of stages of given type
            stage.muscian_performing = self
            perform_time = randInt(t1,t2)

            stage.endtime.update(perform_time)
            sleep(perform_time)
            if stage.singer_perforing:
                #this endtime will be new time set by singer
                sleep_till(stage.endtime)
            return stage.index

        if no_stage_found:
            return -1
```
