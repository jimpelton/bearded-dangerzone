

#include <bd/volume/block.h>

#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#include <iostream>
#include <stack>
#include <fstream>


class TimerStack
{
public:
    TimerStack();
    ~TimerStack();

    //////////////////////////////////////////////////////////////////////////
    /// \brief Push the current time onto the stack.
    //////////////////////////////////////////////////////////////////////////
    void push_time();

    //////////////////////////////////////////////////////////////////////////
    /// \brief Pop the top time and return as double in milliseconds elapsed since
    /// the top was pushed.
    //////////////////////////////////////////////////////////////////////////
    double pop_elapsed();

private:
    std::stack<double> m_times;  ///< Times pushed

#ifdef WIN32
    LARGE_INTEGER m_win_frequency;
#endif
};

TimerStack::TimerStack()
{
#ifdef WIN32
    QueryPerformanceFrequency(&m_win_frequency);
#endif 
}

TimerStack::~TimerStack() { }

void TimerStack::push_time()
{
#ifdef WIN32
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    m_times.push(static_cast<double>(1e9 * now.QuadPart / m_win_frequency.QuadPart));
#endif
}

double TimerStack::pop_elapsed()
{
    double then{ m_times.top() };
    double now_d{ 0.0 };

#ifdef WIN32
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    now_d = 1e9 * now.QuadPart / m_win_frequency.QuadPart;
#endif    
    m_times.pop();
    return (now_d - then) * 1e-6;
}

//////////////////////////////////////////////////////////////////////////////

TimerStack g_timers;

//////////////////////////////////////////////////////////////////////////////
void createMats(std::vector<glm::mat4> &mats, size_t blocksPerDim)
{
    g_timers.push_time();

    glm::vec3 wld_dims{ 1.0f / glm::vec3(blocksPerDim) };

    for (auto bz = 0ul; bz < blocksPerDim; ++bz)
    for (auto by = 0ul; by < blocksPerDim; ++by)
    for (auto bx = 0ul; bx < blocksPerDim; ++bx) {
        glm::u64vec3 blkId{ bx, by, bz };
        glm::vec3 worldLoc{ wld_dims * glm::vec3(blkId) - 0.5f };
        glm::vec3 blk_origin{ worldLoc + (worldLoc + wld_dims) * 0.5f };

        Block blk{ glm::u64vec3(bx, by, bz), wld_dims, blk_origin };
        mats.push_back(blk.transform().matrix());
    }

    double elapsed{ g_timers.pop_elapsed() };
    std::cout << "createMats(): " << elapsed << std::endl;
}


//////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    size_t blocksPerDim{ 0 };
    if (argc >= 2)
    {
        blocksPerDim = atol(argv[1]);
    } 
    else {
        std::cout << "Usage: matrix_benchmark <blocks-per-dim>" << std::endl;
        std::cout << "Exiting..." << std::endl;
        exit(1);
    }

    g_timers.push_time();
    std::vector<glm::mat4> mats;
    createMats(mats, blocksPerDim);
    
    glm::mat4 viewMatrix { glm::lookAt(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) };

    g_timers.push_time();
    for (glm::mat4 &m : mats) {
        m = m * viewMatrix;
    }

    double mults = g_timers.pop_elapsed();
    double total = g_timers.pop_elapsed();
    std::cout << "Multiply " << mats.size() << " mats: " << mults << std::endl;
    std::cout << "Total time: " << total << std::endl;
    std::cout << "Size of matrix data: " << mats.size() * sizeof(glm::mat4) << std::endl;

    std::cout << "Writing to file... " << std::endl;
    std::ofstream outFile("mats.txt");
    for (const glm::mat4 &m : mats)
    {
        outFile << glm::to_string(m) << "\n";
    }
    outFile.flush();
    outFile.close();
    std::cout << "Done, bye!" << std::endl;

    return 0;
}
