#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(TestEm5)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

# Find ROOT
find_package(ROOT REQUIRED
    Gpad
    Hist
    Tree
)


add_executable(G4MScInThinLayerTest
    G4MScInThinLayerTest.cc
    src/ActionInitialization.cc
    src/DetectorConstruction.cc
    src/DetectorMessenger.cc
    src/EventAction.cc
    src/HistoManager.cc
    src/PhysicsList.cc
    src/PhysicsListMessenger.cc
    src/PhysListEmStandard.cc
    src/PhysListEmStandardSSM.cc
    src/PrimaryGeneratorAction.cc
    src/PrimaryGeneratorMessenger.cc
    src/RMSPlotter.cc
    src/RunAction.cc
    src/Run.cc
    src/StackingAction.cc
    src/StackingMessenger.cc
    src/StepMax.cc
    src/StepMaxMessenger.cc
    src/SteppingAction.cc
    src/SteppingVerbose.cc
    src/TrackingAction.cc
)
target_include_directories(G4MScInThinLayerTest
    PRIVATE include
)

target_link_libraries(G4MScInThinLayerTest
    PRIVATE
        ${Geant4_LIBRARIES}
        ROOT::Gpad
        ROOT::Hist
        ${TBB_LIBRARIES}
)

target_compile_definitions(G4MScInThinLayerTest PUBLIC G4VERSION_NUM=${Geant4_VERSION_MAJOR})

install(TARGETS G4MScInThinLayerTest)
install(
    PROGRAMS
        scripts/G4MScInThinLayerTest.py
    TYPE BIN
)
