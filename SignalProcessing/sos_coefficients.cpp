#include "sos_coefficients.h"

#include <array>
#include "../defines.h"

using namespace std;

namespace sos {

    array<array<double,6>,NB_SOS> SOS_0p8kHz = {{
        {1.32694404496e-19, 2.65388808993e-19, 1.32694404496e-19, 1.0, -1.9609813343, 0.974077401263},
        {1.0, 2.0, 1.0, 1.0, -1.96280238295, 0.974987031638},
        {1.0, 2.0, 1.0, 1.0, -1.96176529412, 0.975807122313},
        {1.0, 2.0, 1.0, 1.0, -1.9667965473, 0.978188559985},
        {1.0, 2.0, 1.0, 1.0, -1.96531420904, 0.980230467262},
        {1.0, -2.0, 1.0, 1.0, -1.97240154891, 0.983176298884},
        {1.0, -2.0, 1.0, 1.0, -1.97141190601, 0.987020127105},
        {1.0, -2.0, 1.0, 1.0, -1.97904523629, 0.989409570666},
        {1.0, -2.0, 1.0, 1.0, -1.97944299682, 0.995464574014},
        {1.0, -2.0, 1.0, 1.0, -1.98620705461, 0.996382126119}
    }};
    array<array<double,6>,NB_SOS> SOS_1kHz = {{
        {1.29899946197e-18, 2.59799892395e-18, 1.29899946197e-18, 1.0, -1.94680121217, 0.967474963211},
        {1.0, 2.0, 1.0, 1.0, -1.94937179363, 0.968609817147},
        {1.0, 2.0, 1.0, 1.0, -1.94747078781, 0.969641395423},
        {1.0, 2.0, 1.0, 1.0, -1.95461974128, 0.972614294685},
        {1.0, 2.0, 1.0, 1.0, -1.95161668487, 0.975180186075},
        {1.0, -2.0, 1.0, 1.0, -1.96183110211, 0.978861982656},
        {1.0, -2.0, 1.0, 1.0, -1.95901403409, 0.983691221569},
        {1.0, -2.0, 1.0, 1.0, -1.97028694298, 0.986682539707},
        {1.0, -2.0, 1.0, 1.0, -1.96893765388, 0.994295401256},
        {1.0, -2.0, 1.0, 1.0, -1.97933550196, 0.995446339318}
    }};
    array<array<double,6>,NB_SOS> SOS_1p25kHz= {{
        {1.26478880244e-17, 2.52957760489e-17, 1.26478880244e-17, 1.0, -1.92662937489, 0.959225945269},
        {1.0, 2.0, 1.0, 1.0, -1.93029810265, 0.960637365358},
        {1.0, 2.0, 1.0, 1.0, -1.92697326648, 0.961937005989},
        {1.0, 2.0, 1.0, 1.0, -1.93724288609, 0.965637210008},
        {1.0, 2.0, 1.0, 1.0, -1.93167978074, 0.968863743363},
        {1.0, -2.0, 1.0, 1.0, -1.94655664624, 0.97345296646},
        {1.0, -2.0, 1.0, 1.0, -1.94053911442, 0.979520674495},
        {1.0, -2.0, 1.0, 1.0, -1.95733709722, 0.983256884978},
        {1.0, -2.0, 1.0, 1.0, -1.9527173354, 0.992827252638},
        {1.0, -2.0, 1.0, 1.0, -1.96876954685, 0.994268343813}
    }};
    array<array<double,6>,NB_SOS> SOS_1p6kHz= {{
        {1.22320295331e-16, 2.44640590663e-16, 1.22320295331e-16, 1.0, -1.89762811768, 0.948939732423},
        {1.0, 2.0, 1.0, 1.0, -1.90291549396, 0.950687369428},
        {1.0, 2.0, 1.0, 1.0, -1.89727918034, 0.952329575327},
        {1.0, 2.0, 1.0, 1.0, -1.91217378705, 0.956915491542},
        {1.0, 2.0, 1.0, 1.0, -1.90238786375, 0.960978898352},
        {1.0, -2.0, 1.0, 1.0, -1.92425057086, 0.966676906976},
        {1.0, -2.0, 1.0, 1.0, -1.91280263556, 0.974304029546},
        {1.0, -2.0, 1.0, 1.0, -1.93801554279, 0.978954746952},
        {1.0, -2.0, 1.0, 1.0, -1.92759931049, 0.990985638377},
        {1.0, -2.0, 1.0, 1.0, -1.95245256378, 0.992784999708}
    }};
    array<array<double,6>,NB_SOS> SOS_2kHz= {{
        {1.17309709123e-15, 2.34619418245e-15, 1.17309709123e-15, 1.0, -1.85555232755, 0.936144422503},
        {1.0, 2.0, 1.0, 1.0, -1.86323325943, 0.938294610887},
        {1.0, 2.0, 1.0, 1.0, -1.85389488392, 0.940380471832},
        {1.0, 2.0, 1.0, 1.0, -1.87566396001, 0.946029166067},
        {1.0, 2.0, 1.0, 1.0, -1.85902913443, 0.951161404553},
        {1.0, -2.0, 1.0, 1.0, -1.89138188221, 0.958195505839},
        {1.0, -2.0, 1.0, 1.0, -1.8709349797, 0.967793323461},
        {1.0, -2.0, 1.0, 1.0, -1.90897505475, 0.973552662183},
        {1.0, -2.0, 1.0, 1.0, -1.88864767151, 0.98867921767},
        {1.0, -2.0, 1.0, 1.0, -1.92717968528, 0.990916085653}
    }};
    array<array<double,6>,NB_SOS> SOS_2p5kHz= {{
        {1.11338641156e-14, 2.22677282312e-14, 1.11338641156e-14, 1.0, -1.79408407108, 0.92027643355},
        {1.0, 2.0, 1.0, 1.0, -1.80530300023, 0.922897124335},
        {1.0, 2.0, 1.0, 1.0, -1.79011573954, 0.925569536641},
        {1.0, 2.0, 1.0, 1.0, -1.82209609716, 0.932463850938},
        {1.0, 2.0, 1.0, 1.0, -1.79452707025, 0.938979690155},
        {1.0, -2.0, 1.0, 1.0, -1.84261243293, 0.947588221205},
        {1.0, -2.0, 1.0, 1.0, -1.80754625559, 0.959692864492},
        {1.0, -2.0, 1.0, 1.0, -1.86509550066, 0.96676868875},
        {1.0, -2.0, 1.0, 1.0, -1.82826965443, 0.985797596411},
        {1.0, -2.0, 1.0, 1.0, -1.88797926834, 0.988558946097}
    }};
    array<array<double,6>,NB_SOS> SOS_3p15kHz= {{
        {1.04319463479e-13, 2.08638926959e-13, 1.04319463479e-13, 1.0, -1.70392849231, 0.900672834722},
        {1.0, 2.0, 1.0, 1.0, -1.7203497559, 0.903821630799},
        {1.0, 2.0, 1.0, 1.0, -1.69606425259, 0.907293180012},
        {1.0, 2.0, 1.0, 1.0, -1.74313040354, 0.915590663859},
        {1.0, 2.0, 1.0, 1.0, -1.69839651647, 0.923935675969},
        {1.0, -2.0, 1.0, 1.0, -1.76994056041, 0.93433035222},
        {1.0, -2.0, 1.0, 1.0, -1.71157740309, 0.949659581621},
        {1.0, -2.0, 1.0, 1.0, -1.7986081749, 0.958244186023},
        {1.0, -2.0, 1.0, 1.0, -1.73494648806, 0.98221059015},
        {1.0, -2.0, 1.0, 1.0, -1.82720164463, 0.985580660292}
    }};
    array<array<double,6>,NB_SOS> SOS_4kHz= {{
        {9.62062598155e-13, 1.92412519631e-12, 9.62062598155e-13, 1.0, -1.57170117166, 0.876569694162},
        {1.0, 2.0, 1.0, 1.0, -1.5956809715, 0.880268261579},
        {1.0, 2.0, 1.0, 1.0, -1.5575191797, 0.884873977882},
        {1.0, 2.0, 1.0, 1.0, -1.62660474393, 0.894639556667},
        {1.0, 2.0, 1.0, 1.0, -1.55547035322, 0.905479139291},
        {1.0, -2.0, 1.0, 1.0, -1.66156599125, 0.917761199881},
        {1.0, -2.0, 1.0, 1.0, -1.56687473826, 0.937314150938},
        {1.0, -2.0, 1.0, 1.0, -1.69790099928, 0.94751608845},
        {1.0, -2.0, 1.0, 1.0, -1.591609317, 0.977771399137},
        {1.0, -2.0, 1.0, 1.0, -1.73324034396, 0.981805783635}
    }};
    array<array<double,6>,NB_SOS> SOS_5kHz= {{
        {8.7021157354e-12, 1.74042314708e-11, 8.7021157354e-12, 1.0, -1.37884981236, 0.847111966578},
        {1.0, 2.0, 1.0, 1.0, -1.41357293462, 0.851293330764},
        {1.0, 2.0, 1.0, 1.0, -1.35481934663, 0.857593431386},
        {1.0, 2.0, 1.0, 1.0, -1.45534431355, 0.868659325427},
        {1.0, 2.0, 1.0, 1.0, -1.34471367572, 0.883050500666},
        {1.0, -2.0, 1.0, 1.0, -1.50058694776, 0.897032624787},
        {1.0, -2.0, 1.0, 1.0, -1.35082009865, 0.922275967408},
        {1.0, -2.0, 1.0, 1.0, -1.54606907163, 0.933970587162},
        {1.0, -2.0, 1.0, 1.0, -1.37397514538, 0.972329157881},
        {1.0, -2.0, 1.0, 1.0, -1.58890121189, 0.976995460848}
    }};
    array<array<double,6>,NB_SOS> SOS_6p3kHz= {{
        {7.68831776735e-11, 1.53766355347e-10, 7.68831776735e-11, 1.0, -1.10139541471, 0.811384865165},
        {1.0, 2.0, 1.0, 1.0, -1.15082229619, 0.815784850841},
        {1.0, 2.0, 1.0, 1.0, -1.06273402213, 0.824771988589},
        {1.0, 2.0, 1.0, 1.0, -1.20647830199, 0.836446871211},
        {1.0, 2.0, 1.0, 1.0, -1.03912826425, 0.856184082083},
        {1.0, -2.0, 1.0, 1.0, -1.26404036316, 0.871014458542},
        {1.0, -2.0, 1.0, 1.0, -1.03411753819, 0.904251050511},
        {1.0, -2.0, 1.0, 1.0, -1.31963501507, 0.916756592498},
        {1.0, -2.0, 1.0, 1.0, -1.04999650961, 0.96576355903},
        {1.0, -2.0, 1.0, 1.0, -1.36975500552, 0.970807994924}
    }};
    array<array<double,6>,NB_SOS> SOS_8kHz= {{
        {6.60330411795e-10, 1.32066082359e-09, 6.60330411795e-10, 1.0, -0.712493641239, 0.768488260162},
        {1.0, 2.0, 1.0, 1.0, -0.780777996346, 0.772412776974},
        {1.0, 2.0, 1.0, 1.0, -0.653531436914, 0.785954619986},
        {1.0, 2.0, 1.0, 1.0, -0.85292184216, 0.796396800241},
        {1.0, 2.0, 1.0, 1.0, -0.609263428715, 0.824750764633},
        {1.0, -2.0, 1.0, 1.0, -0.923798861116, 0.838092987778},
        {1.0, -2.0, 1.0, 1.0, -0.584575424954, 0.883245228965},
        {1.0, -2.0, 1.0, 1.0, -0.988810097274, 0.894601171911},
        {1.0, -2.0, 1.0, 1.0, -0.583568604764, 0.958073839915},
        {1.0, -2.0, 1.0, 1.0, -1.04369811976, 0.962714449027}
    }};
    array<array<double,6>,NB_SOS> SOS_10kHz= {{
        {5.48433368822e-09, 1.09686673764e-08, 5.48433368822e-09, 1.0, -0.1921667064, 0.717717888939},
        {1.0, 2.0, 1.0, 1.0, -0.28195982996, 0.719490612736},
        {1.0, 2.0, 1.0, 1.0, -0.107978210652, 0.74138245318},
        {1.0, -2.0, 1.0, 1.0, -0.370950529761, 0.74611105819},
        {1.0, -2.0, 1.0, 1.0, -0.0356529460848, 0.78958187372},
        {1.0, -2.0, 1.0, 1.0, -0.453083860519, 0.795658385047},
        {1.0, 2.0, 1.0, 1.0, 0.0189744044045, 0.860120287651},
        {1.0, -2.0, 1.0, 1.0, -0.5228722544, 0.865344003004},
        {1.0, 2.0, 1.0, 1.0, 0.0503534717333, 0.949618619118},
        {1.0, -2.0, 1.0, 1.0, -0.575090902368, 0.951785669024}
    }};
    array<array<double,6>,NB_SOS> SOS_12p5kHz= {{
        {4.38012206813e-08, -8.76024413627e-08, 4.38012206813e-08, 1.0, 0.338805915232, 0.654464770495},
        {1.0, -2.0, 1.0, 1.0, 0.448069722589, 0.659139961176},
        {1.0, -2.0, 1.0, 1.0, 0.237948328336, 0.681073714771},
        {1.0, -2.0, 1.0, 1.0, 0.557844385966, 0.693551554749},
        {1.0, -2.0, 1.0, 1.0, 0.153124581013, 0.73842474595},
        {1.0, 2.0, 1.0, 1.0, 0.661119677372, 0.754509488206},
        {1.0, 2.0, 1.0, 1.0, 0.0911665518546, 0.824438733548},
        {1.0, 2.0, 1.0, 1.0, 0.752248721867, 0.83838304477},
        {1.0, 2.0, 1.0, 1.0, 0.05864376216, 0.936007610998},
        {1.0, 2.0, 1.0, 1.0, 0.826209665254, 0.94188606617}
    }};
    array<array<double,6>,NB_SOS> SOS_16kHz= {{
        {3.34538691612e-07, -6.69077383224e-07, 3.34538691612e-07, 1.0, 0.995892015562, 0.570701908537},
        {1.0, -2.0, 1.0, 1.0, 0.893762705954, 0.587103801692},
        {1.0, -2.0, 1.0, 1.0, 1.11850222157, 0.597075546217},
        {1.0, -2.0, 1.0, 1.0, 0.825041214547, 0.649660106301},
        {1.0, -2.0, 1.0, 1.0, 1.24654986308, 0.65653652549},
        {1.0, 2.0, 1.0, 1.0, 1.36966426243, 0.737724032949},
        {1.0, 2.0, 1.0, 1.0, 0.798481545402, 0.757540364905},
        {1.0, 2.0, 1.0, 1.0, 1.48437445151, 0.833366127917},
        {1.0, 2.0, 1.0, 1.0, 0.821821450131, 0.908930690032},
        {1.0, 2.0, 1.0, 1.0, 1.59101758476, 0.94121389669}
    }};
    array<array<double,6>,NB_SOS> SOS_20kHz= {{
        {9.02674009001e-07, -1.805348018e-06, 9.02674009001e-07, 1.0, 1.00389105058, 0.252918287938},
        {1.0, -2.0, 1.0, 1.0, 1.12328767123, 0.342465753425},
        {1.0, -2.0, 1.0, 1.0, 1.07692307692, 0.384615384615},
        {1.0, -2.0, 1.0, 1.0, 1.34426229508, 0.606557377049},
        {1.0, -2.0, 1.0, 1.0, 1.63278688525, 0.84262295082},
        {1.0, 2.0, 1.0, 1.0, 2.0, 1.0},
        {1.0, 2.0, 1.0, 1.0, 2.0, 1.0},
        {1.0, 2.0, 1.0, 1.0, 2.0, 1.0},
        {1.0, 2.0, 1.0, 1.0, 2.0, 1.0},
        {1.0, 2.0, 1.0, 1.0, 2.0, 1.0}
    }};
    array<array<double,6>,NB_SOS> SOS_ALIASING= {{
        {1.31821644002e-17, 2.63643288003e-17, 1.31821644002e-17, 1.0, -1.45414326339, 0.528976710083},
        {1.0, 2.0, 1.0, 1.0, -1.46262514113, 0.53789508412},
        {1.0, 2.0, 1.0, 1.0, -1.47967421693, 0.555821543282},
        {1.0, 2.0, 1.0, 1.0, -1.50545523446, 0.582929309321},
        {1.0, 2.0, 1.0, 1.0, -1.54019947634, 0.619461567158},
        {1.0, 2.0, 1.0, 1.0, -1.58418264998, 0.665708212751},
        {1.0, 2.0, 1.0, 1.0, -1.63769006221, 0.721969235427},
        {1.0, 2.0, 1.0, 1.0, -1.70096433194, 0.788499739815},
        {1.0, 2.0, 1.0, 1.0, -1.77412981843, 0.865430484977},
        {1.0, 2.0, 1.0, 1.0, -1.85708758395, 0.952657441623}
    }};

    array<array<double,6>,NB_SOS> getCoefficients(int freq)
    {
        switch (freq) {
        case FREQ_20kHz:
            return SOS_20kHz;
        case FREQ_16kHz:
        case FREQ_1p6kHz:
        case FREQ_160Hz:
        case FREQ_16Hz:
            return SOS_16kHz;
        case FREQ_12p5kHz:
        case FREQ_1p25kHz:
        case FREQ_125Hz:
        case FREQ_12p5Hz:
            return SOS_12p5kHz;
        case FREQ_10kHz:
        case FREQ_1kHz:
        case FREQ_100Hz:
        case FREQ_10Hz:
            return SOS_10kHz;
        case FREQ_8kHz:
        case FREQ_800Hz:
        case FREQ_80Hz:
        case FREQ_8Hz:
            return SOS_8kHz;
        case FREQ_6p3kHz:
        case FREQ_630Hz:
        case FREQ_63Hz:
        case FREQ_6p3Hz:
            return SOS_6p3kHz;
        case FREQ_5kHz:
        case FREQ_500Hz:
        case FREQ_50Hz:
        case FREQ_5Hz:
            return SOS_5kHz;
        case FREQ_4kHz:
        case FREQ_400Hz:
        case FREQ_40Hz:
        case FREQ_4Hz:
            return SOS_4kHz;
        case FREQ_3p15kHz:
        case FREQ_315Hz:
        case FREQ_31p5Hz:
        case FREQ_3p15Hz:
            return SOS_3p15kHz;
        case FREQ_2kHz:
        case FREQ_200Hz:
        case FREQ_20Hz:
        case FREQ_2Hz:
            return SOS_2kHz;
        case FREQ_1p6Hz:
            return SOS_1p6kHz;
        case FREQ_1p25Hz:
            return SOS_1p25kHz;
        case FREQ_1Hz:
            return SOS_1kHz;
        case FREQ_0p8Hz:
            return SOS_0p8kHz;
        default: // who knows ...
            return SOS_20kHz;
        }
    }

}