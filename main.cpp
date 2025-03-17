#include <iostream>
#include <string>
#include <limits>
#include <vector>

namespace Constants {
    constexpr int cancelValue = 0;
    constexpr int lowestPriority = 1;
    constexpr int highestPriority = 10;
}

namespace HelperFunctions {
    int numberGetter() {
        bool stopLoop { false };
        int a {};
        while (!stopLoop) {
            if ( std::cin >> a && std::isspace( std::cin.peek()) ) {
                stopLoop = true;

                // Stops from allowing user to enter 2+ numbers at once (only takes the first number given. Ignores all other characters in buffer until a newline character is read)
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            std::cout << "Invalid input. Please enter value again." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return a;
    }

    void getNameAndTransformToUpper(std::string &a) {
        std::getline(std::cin >> std::ws, a);
        for (int i = 0; i < a.length(); i++)
            a[i] = toupper(a[i]);
    }
}

class Job {
private:
    int priority {};
    std::string jobName {};

public:
    Job(std::string &name, int choice) {
        setPriority(choice);
        setJobName(name);
    }

    void setPriority(int choice) {
        priority = choice;
    }

    void setJobName(std::string &name) {
        jobName = name;
    }

    std::string getJobName() {
        return jobName;
    }

    int getJobPriority() {
        return priority;
    }
};

class PriorityQueue {
protected:
    std::vector<Job> jobQueue;
    std::vector<Job> sortedList;

protected:
    bool uniqueName(std::string &n) {
        bool unique {true};
        for (auto &e : jobQueue) {
            if (n == e.getJobName()) {
                unique = false;
                std::cout << "Name not unique" << std::endl;
                break;
            }
        }
        return unique;
    }

    bool choiceWithinRange(int p) {
        bool withinRange {false};
        if (p >= 0 && p <= Constants::highestPriority)
            withinRange = true;
        else
            std::cout << "Choice not within range 0 - 10." << std::endl;

        return withinRange;
    }

    void heapifyWhenAdding(int lastIndex) {
        auto parentOfLastIndex = (lastIndex-1)/2;

        if (parentOfLastIndex >= 0) {
            if (jobQueue[parentOfLastIndex].getJobPriority() < jobQueue[lastIndex].getJobPriority()) {
                std::swap(jobQueue[parentOfLastIndex], jobQueue[lastIndex]);
                heapifyWhenAdding(parentOfLastIndex);
            }
        }
    }

    void heapify(std::vector<Job> &list, int totalSize, int parentIndex) {
        auto largest = parentIndex;
        auto leftChild = (2 * parentIndex) + 1;
        auto rightChild = leftChild + 1;

        if (leftChild < totalSize) {
            if (list[largest].getJobPriority() < list[leftChild].getJobPriority())
                largest = leftChild;
        }

        if (rightChild < totalSize) {
            if (list[largest].getJobPriority() < list[rightChild].getJobPriority())
                largest = rightChild;
        }

        if(largest != parentIndex){
            std::swap(list[largest],list[parentIndex]);
            heapify(list, totalSize,largest);
        }
    }

    void sortQueue() {
        sortedList.clear();
        sortedList = jobQueue;
        auto size = static_cast<int>(sortedList.size());
        for (int i = size - 1; i >= 0; i--) {
            std::swap(sortedList[i], sortedList[0]);
            heapify(sortedList,i,0);
        }
    }

    void buildHeap(int totalSize) {
        for (int i = (totalSize / 2) -1; i >= 0; i--) {
            heapify(jobQueue, totalSize, i);
        }
    }

public:
    [[nodiscard]] bool queueEmpty() {
        return jobQueue.empty();
    }

    void printHighestPriority() {
        std::cout << "Highest priority: " << jobQueue[0].getJobName() << " (Priority: " << jobQueue[0].getJobPriority() << ")" << std::endl;
    }

    void printQueue() {
        if (jobQueue.empty()) {
            std::cout << "Queue is empty." << std::endl;
            return;
        } else {
            sortQueue();
            std::cout << "Queue contains: [";
            auto lastIndex = static_cast<int>(sortedList.size()) - 1;
            for (int i = 0; i <= lastIndex; ++i) {
                if (i > 0)
                    std::cout << ", ";
                std::cout << "\"" << sortedList[lastIndex - i].getJobName() << "\"";
            }
            std::cout << "]" << std::endl;
            printHighestPriority();
        }
    }

    void addJob() {
        std::cout << "\nTo add a job, you must first enter a unique name for the job, then its priority."
                     "\nTo cancel adding a job, type 0 when setting priority. " <<
                     "Lowest priority: " << Constants::lowestPriority << " and highest priority: " <<
                     Constants::highestPriority <<std::endl;

        std::string name;
        do {
            std::cout << "Enter job name: " << std::endl;
            HelperFunctions::getNameAndTransformToUpper(name);
        } while (!uniqueName(name));

        int choice {};
        do {
            std::cout << "Enter priority (enter 0 to cancel add operation): " << std::endl;
            choice = HelperFunctions::numberGetter();
        } while (!choiceWithinRange(choice));


        if (choice == Constants::cancelValue) {
            std::cout << "Cancelling add job operation..." << std::endl;
            return;
        } else {
            Job temp(name, choice);
            std::cout << "Adding job to queue..." << std::endl;
            auto oldSize = jobQueue.size();

            jobQueue.push_back(temp);

            if(oldSize > 0) {
                auto lastIndex = static_cast<int>(jobQueue.size() - 1);
                heapifyWhenAdding(lastIndex);
            }
            printQueue();
        }
    }

    void updateExistingJob() {
         if (jobQueue.empty()) {
             std::cout << "Queue is empty." << std::endl;
             return;
         } else {
             std::cout << "Enter job name to find:" << std::endl;
             std::string name;
             HelperFunctions::getNameAndTransformToUpper(name);

             bool nameFound{};
             auto it = jobQueue.begin();
             while (it != jobQueue.end()) {
                 if (name == it->getJobName()) {
                     nameFound = true;
                     break;
                 }
                 ++it;
             }

             if (nameFound) {
                 int choice{};
                 do {
                     std::cout << "Enter new priority (enter 0 to cancel update priority operation): " << std::endl;
                     choice = HelperFunctions::numberGetter();
                 } while (!choiceWithinRange(choice));

                 if (choice == Constants::cancelValue) {
                     std::cout << "Cancelling update job priority operation..." << std::endl;
                     return;
                 } else {
                     std::cout << "Updating job priority..." << std::endl;
                     it->setPriority(choice);
                     auto size = static_cast<int>(jobQueue.size());
                     if (size > 1)
                        buildHeap(size);
                     printQueue();
                 }
             } else {
                std::cout << "Name not found. Exiting update job priority operation..." << std::endl;
                return;
             }
         }
    }

    void processNextJob() {
        if (jobQueue.empty()) {
            std::cout << "Queue is empty" << std::endl;
            return;
        } else {
            std::cout << "Processing: " << jobQueue[0].getJobName() << std::endl;
            auto movingIndex = 0;
            auto lastIndex = jobQueue.size() - 1;
            if (lastIndex > 0) {
                std::swap(jobQueue[movingIndex], jobQueue[lastIndex]);
                jobQueue.pop_back();
                --lastIndex;

                auto size = static_cast<int>(jobQueue.size());
                heapify(jobQueue, size, 0);
                
            } else
                jobQueue.pop_back();
            printQueue();
        }
    }
};

class ProgramController {
private:
    bool programRunning {};
public:
    ProgramController(): programRunning(true) {};

    void displayMenu() {
        std::cout << "\n=== Menu ==="<<std::endl;
        std::cout << "1. Add new job"<<std::endl;
        std::cout << "2. Update existing job priority"<<std::endl;
        std::cout << "3. Display print jobs"<<std::endl;
        std::cout << "4. Print the name of job with highest priority"<<std::endl;
        std::cout << "5. Process the next job" << std::endl;
        std::cout << "6. Exit"<<std::endl;
        std::cout << "Please choose from the menu: "<<std::endl;
    }

    [[nodiscard]] bool getProgramState() const {
        return programRunning;
    }

    void endProgram() {
        std::cout << "Exiting the program" << std::endl;
        programRunning = false;
    }
};

int main() {
    ProgramController control;
    PriorityQueue q;

    while (control.getProgramState()) {
        control.displayMenu();
        int choice {HelperFunctions::numberGetter()};

        switch (choice) {
            case 1: // Add job
                q.addJob();
                break;
            case 2: // Update existing job priority
                q.updateExistingJob();
                break;
            case 3: // Print sorted jobs
                q.printQueue();
                break;
            case 4: // Print job with the highest priority
                if (q.queueEmpty())
                    std::cout << "Queue empty." << std::endl;
                else
                    q.printHighestPriority();
                break;
            case 5: // Process job with the highest priority
                q.processNextJob();
                break;
            case 6: // End program
                control.endProgram();
                break;
            default:
                std::cout << "Invalid choice. Enter new value." << std::endl;
        }
    }

    return 0;
}
