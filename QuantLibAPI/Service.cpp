#include "Service.h"
#include <string>
#pragma warning( disable: 4819 )
#include <ql/quantlib.hpp>

using namespace QuantLib;
using std::string;

void Service::initRestOpHandlers() {
    _listener.support(methods::POST, std::bind(&Service::handlePost, this, std::placeholders::_1));
}

void Service::handlePost(http_request message) {

    std::vector<utility::string_t> path = requestPath(message);
    std::string pathEndpoint = utility::conversions::to_utf8string(path[0]);

    message.extract_json().then([=](pplx::task<json::value> task)
        {
            try
            {
                json::value val = task.get();


                if (pathEndpoint == "blacksholes" || pathEndpoint == "montecarlo") {

                    double S0 = val[U("S0")].as_number().to_double();
                    double K = val[U("K")].as_number().to_double();

                    std::cout << "S0 = " << S0 << " K = " << K << std::endl;

                    calendar calendar = target();
                    daycounter daycounter = business252();
                    date t0(20, jan, 2020);
                    settings::instance().evaluationdate() = t0;
                    date t(26, mar, 2021);

                    Option::Type type(Option::Call);
                    Real S0 = val[U("S0")].as_number().to_double();
                    Real K = val[U("K")].as_number().to_double();

                    Spread q = val[U("Q")].as_number().to_double();
                    Rate r = val[U("r")].as_number().to_double();

                    Volatility sigma = val[U("sigma")].as_number().to_double();

                    Handle<Quote> underlyingH(
                        boost::shared_ptr<Quote>(
                            new SimpleQuote(S0)
                            )
                    );

                    Handle<YieldTermStructure> flatDividendTS(
                        boost::shared_ptr<YieldTermStructure>(
                            new FlatForward(t0, q, dayCounter)
                            )
                    );

                    Handle<YieldTermStructure> flatTermStructure(
                        boost::shared_ptr<YieldTermStructure>(
                            new FlatForward(t0, r, dayCounter)
                            )
                    );

                    Handle<BlackVolTermStructure> flatVolTS(
                        boost::shared_ptr<BlackVolTermStructure>(
                            new BlackConstantVol(t0, calendar, sigma, dayCounter)
                            )
                    );

                    boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
                        new BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS)
                    );

                    boost::shared_ptr<Exercise> europenExercise(
                        new EuropeanExercise(T)
                    );

                    boost::shared_ptr<StrikedTypePayoff> payoff(
                        new PlainVanillaPayoff(type, K)
                    );

                    VanillaOption europenOption(payoff, europenExercise);

                    if (pathEndpoint == "blacksholes") {
                        std::cout << "POST api/v1/blacksholes"<<std::endl;

                        europenOption.setPricingEngine(
                            boost::shared_ptr<PricingEngine>(
                                new AnalyticEuropeanEngine(bsmProcess)
                                )
                        );

                        Real OptionPrice = europenOption.NPV();

                        message.reply(status_codes::OK, OptionPrice);
                    }

                    if (pathEndpoint == "montecarlo") {
                        std::cout << "POST api/v1/blacksholes" << std::endl;
                        europenoption.setpricingengine(
                            boost::shared_ptr<pricingengine>(
                                new mceuropeengine<pseudorandom>(
                                    bsmprocess,
                                    val[u("timestamps")].as_number().to_int32(),
                                    null<size>(),
                                    false,
                                    false,
                                    val[u("samples")].as_number().to_int32(),
                                    null<real>(),
                                    null<size>(), 
                                    seedgenerator::instance().get()
                                    )
                                )
                        );

                        real OptionPrice = europenoption.npv();
                        real error = europenoption.errorestimate();

                        message.reply(status_codes::OK, json::value::number(OptionPrice));
                    }

                }
                else {
                    message.reply(status_codes::BadRequest);
                }


            }
            catch (std::exception& e) {
                message.reply(status_codes::BadRequest);
            }
        });
    
}
