#include "p25_recorder_fsk4_demod.h"

p25_recorder_fsk4_demod_sptr make_p25_recorder_fsk4_demod() {
  p25_recorder_fsk4_demod *recorder = new p25_recorder_fsk4_demod();
  // recorder->initialize(src, gr::blocks::nonstop_wavfile_sink_impl::make(1,
  // 8000, 16, false));
  recorder->initialize();
  return gnuradio::get_initial_sptr(recorder);
}

p25_recorder_fsk4_demod::p25_recorder_fsk4_demod()
    : gr::hier_block2("p25_recorder_fsk4_demod",
                      gr::io_signature::make(1, 1, sizeof(gr_complex)),
                      gr::io_signature::make(1, 1, sizeof(float))) {}

p25_recorder_fsk4_demod::~p25_recorder_fsk4_demod() {}

void p25_recorder_fsk4_demod::reset() {
  /*
  pll_freq_lock->update_gains();
  pll_freq_lock->frequency_limit();
  pll_freq_lock->phase_wrap();
  fsk4_demod->reset();*/
  // pll_demod->set_phase(0);
}

void p25_recorder_fsk4_demod::initialize() {
  const double phase1_channel_rate =
      phase1_symbol_rate * phase1_samples_per_symbol;
  const double pi = M_PI;

  // FSK4: Phase Loop Lock - can only be Phase 1, so locking at that rate.
  double freq_to_norm_radians = pi / (phase1_channel_rate / 2.0);
  double fc = 0.0;
  double fd = 600.0;
  double pll_demod_gain = 1.0 / (fd * freq_to_norm_radians);
  double samples_per_symbol = 5;
  pll_freq_lock = gr::analog::pll_freqdet_cf::make(
      (phase1_symbol_rate / 2.0 * 1.2) * freq_to_norm_radians,
      (fc + (3 * fd * 1.9)) * freq_to_norm_radians,
      (fc + (-3 * fd * 1.9)) * freq_to_norm_radians);
  pll_amp = gr::blocks::multiply_const_ff::make(pll_demod_gain * 1.0);

  // FSK4: noise filter - can only be Phase 1, so locking at that rate.
  baseband_noise_filter_taps = gr::filter::firdes::low_pass_2(
      1.0, phase1_channel_rate, phase1_symbol_rate / 2.0 * 1.175,
      phase1_symbol_rate / 2.0 * 0.125, 20.0, gr::filter::firdes::WIN_KAISER,
      6.76);
  noise_filter =
      gr::filter::fft_filter_fff::make(1.0, baseband_noise_filter_taps);

  // FSK4: Symbol Taps
  double symbol_decim = 1;

  for (int i = 0; i < samples_per_symbol; i++) {
    sym_taps.push_back(1.0 / samples_per_symbol);
  }
  sym_filter = gr::filter::fir_filter_fff::make(symbol_decim, sym_taps);

  // FSK4: FSK4 Demod - locked at Phase 1 rates, since it can only be Phase 1
  tune_queue = gr::msg_queue::make(20);
  fsk4_demod = gr::op25_repeater::fsk4_demod_ff::make(
      tune_queue, phase1_channel_rate, phase1_symbol_rate);

  connect(self(), 0, pll_freq_lock, 0);
  connect(pll_freq_lock, 0, pll_amp, 0);
  connect(pll_amp, 0, noise_filter, 0);
  connect(noise_filter, 0, sym_filter, 0);
  connect(sym_filter, 0, fsk4_demod, 0);
  connect(fsk4_demod, 0, self(), 0);
}