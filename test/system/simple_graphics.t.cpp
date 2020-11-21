#include "testing.t.hpp"

#include "app/services/config.hpp"
#include "app/services/graphics.hpp"
#include "lib/widget.hpp"

#include <SkFont.h>
#include <SkPath.h>
#include <SkTypeface.h>
#include <SkTextBlob.h>

#include <string>

using namespace otto;
using namespace otto::services;

//////////////
// Stuff we will need!
SkPaint OTTO_paint(void) {
  SkPaint p;
  p.setAntiAlias(true);
  p.setStyle(SkPaint::kStroke_Style);
  p.setStrokeCap(SkPaint::kRound_Cap);
  p.setStrokeJoin(SkPaint::kRound_Join);
  p.setStrokeWidth(6.f);
  return p;
}

namespace Colours {
  constexpr SkColor Blue = SkColorSetRGB(22, 184, 254);
  constexpr SkColor Green = SkColorSetRGB(22, 254, 101);
  constexpr SkColor Yellow = SkColorSetRGB(254, 245, 22);
  constexpr SkColor Red = SkColorSetRGB(254, 22, 22);
  constexpr SkColor White = SK_ColorWHITE;
  constexpr SkColor Grey50 = SK_ColorGRAY;
  constexpr SkColor Black = SK_ColorBLACK;
}



// Not the real one, just for testing now
SkColor mix(SkColor a, SkColor b, float m) { return a; }

struct ADSR : otto::graphics::Widget<ADSR> {
  float a, d, s, r;
  ADSR(float a_, float d_, float s_, float r_) : a(a_), d(d_), s(s_), r(r_) {}
  void do_draw(SkCanvas& ctx) {
    //If your are using a different anchor/origin than TopLeft for the coordinate system, translate here
    //otto::graphics::Point diff = bounding_box.get_diff(otto::graphics::Anchors::TopLeft, otto::graphics::Anchors::TopLeft);
    //ctx.translate(diff[0], diff[1]);

    const float width = bounding_box.width();
    const float height = bounding_box.height();
    const float spacing = width / 16.f;
    const float max_width = width / 3.f;
    const float aw = max_width * a;
    const float dw = max_width * d;
    const float sh = height * s;
    const float rw = max_width * r;

    SkPaint paint = OTTO_paint();
    paint.setStyle(SkPaint::kStrokeAndFill_Style);

    const float arc_size = 0.9;

    SkPath p;
    p.moveTo(0, height);
    p.quadTo(aw * arc_size, height * arc_size, aw, 0); // curve
    p.lineTo(aw, height);
    p.close();
    paint.setColor(Colours::Blue);
    ctx.drawPath(p, paint);

    p.reset();
    
    p.moveTo(aw + spacing, height);
    p.lineTo(aw + spacing, 0);
    p.quadTo(aw + spacing + dw * (1 - arc_size), (height - sh) * arc_size,
              aw + spacing + dw, height - sh); // curve
    p.lineTo(aw + spacing + dw, height);
    p.close();
    paint.setColor(Colours::Green);
    ctx.drawPath(p, paint);

    p.reset();

    p.moveTo(aw + spacing + dw + spacing, height - sh);
    p.lineTo(width - spacing - rw, height - sh);
    p.lineTo(width - spacing - rw, height);
    p.lineTo(aw + spacing + dw + spacing, height);
    p.close();
    paint.setColor(Colours::Yellow);
    ctx.drawPath(p, paint);

    p.reset();

    p.moveTo(width - rw, height);
    p.lineTo(width - rw, height - sh);
    p.quadTo(width - rw * arc_size, height - sh * (1 - arc_size),
              width, height);
    p.close();
    paint.setColor(Colours::Red);
    ctx.drawPath(p, paint);
  }
};

////////////////

/// For defining frequency ratios
struct Fraction {
  int numerator;
  int denominator;

  constexpr Fraction(int n = 1, int d = 1) : numerator(n), denominator(d) {}

  operator float() const
  {
    return float(numerator) / float(denominator);
  }

  std::string to_string() const
  {
    if (numerator == 0) return fmt::format("0");
    else return fmt::format("{}/{}", numerator, denominator);
  }
};

/// Used for graphics. Defined here for the Algorithm definitions below.
enum struct Drawside {
  left,
  middle,
  right,
  // Number of sides
  n_sides,
};

struct Operatorline {
  int start = 1;
  int end = 2;
  Drawside side = Drawside::middle;
};

struct Algorithm {
  std::array<bool, 4> modulator_flags = {false, false, false, false};
  std::vector<Operatorline> operator_lines;

  Algorithm(std::initializer_list<int> modulator_idx = {}, std::vector<Operatorline> op_lines = {})
    : operator_lines(op_lines)
  {
    for (auto&& idx : modulator_idx) {
      modulator_flags[idx] = true;
    }
  }
};

constexpr std::array<SkColor, 4> operator_colours = {Colours::Red, Colours::Yellow, Colours::Green, Colours::Blue};

const std::array<Fraction, 20> fractions = {{
  {1, 1}, {1, 64}, {1, 32}, {3, 32}, {1, 8}, {5, 16}, {1, 2}, {5, 8},  {2, 1}, {3, 2},
  {3, 4}, {1, 4},  {5, 32}, {1, 16}, {5, 8}, {4, 1},  {7, 4}, {7, 16}, {7, 2}, {0, 1},
}};

/// Defines the algorithms and gives instructions on how to draw them
const std::array<Algorithm, 11> algorithms = {{
  Algorithm({1, 2, 3},
            {
              {3, 2, Drawside::middle},
              {2, 1, Drawside::middle},
              {1, 0, Drawside::middle},
            }),
  Algorithm({1, 2, 3},
            {
              {3, 1, Drawside::left},
              {2, 1, Drawside::right},
              {1, 0, Drawside::middle},
            }),
  Algorithm({1, 2, 3},
            {
              {3, 0, Drawside::left},
              {2, 1, Drawside::middle},
              {1, 0, Drawside::middle},
            }),
  Algorithm({1, 2, 3},
            {
              {3, 2, Drawside::left},
              {3, 1, Drawside::left},
              {2, 0, Drawside::right},
              {1, 0, Drawside::right},
            }),
  Algorithm({2, 3},
            {
              {3, 2, Drawside::middle},
              {2, 1, Drawside::left},
              {2, 0, Drawside::left},
            }),
  Algorithm({2, 3},
            {
              {3, 2, Drawside::middle},
              {2, 1, Drawside::middle},
            }),
  Algorithm({1, 2, 3},
            {
              {3, 0, Drawside::left},
              {2, 0, Drawside::right},
              {1, 0, Drawside::middle},
            }),
  Algorithm({1, 3},
            {
              {3, 2, Drawside::middle},
              {1, 0, Drawside::middle},
            }),
  Algorithm({3},
            {
              {3, 2, Drawside::middle},
              {3, 1, Drawside::right},
              {3, 0, Drawside::left},
            }),
  Algorithm({3},
            {
              {3, 2, Drawside::middle},
            }),
  Algorithm({}),
}};

struct Operators : otto::graphics::Widget<Operators> {
  int algorithm_idx, cur_op;
  std::array<float, 4> activity_levels;
  std::array<SkColor, 4> operator_colours = {Colours::Red, Colours::Yellow, Colours::Green, Colours::Blue};

  Operators(int alg, int op, std::array<float, 4> levels) : algorithm_idx(alg), cur_op(op), activity_levels(levels) {}

  void do_draw(SkCanvas& ctx) {
    // Anchor is TopLeft
    SkPaint paint = OTTO_paint();
    const float width = bounding_box.width();
    const float height = bounding_box.height();
    const float operator_size = width / 2.f;
    const float operator_radius = operator_size / 2.f;
    constexpr float square_scale = 0.9f;
    float space = (height - operator_size) / 3.f;

    // Draw lines between operators
    float x_middle = width / 2.f;
    constexpr float line_shift = 0.f;
    float x_left = x_middle - operator_radius - line_shift;
    float x_right = x_middle + operator_radius + line_shift;
    for (auto&& line : algorithms[algorithm_idx].operator_lines) {
      SkPath path;
      if (line.side == Drawside::left) {
        int y_start = operator_size / 2.f + (3 - line.start) * space;
        int y_end = operator_size / 2.f + (3 - line.end) * space;
        path.moveTo(x_left, y_start);
        path.lineTo(0, y_start);
        path.lineTo(0, y_end);
        path.lineTo(x_left, y_end);
      } else if (line.side == Drawside::right) {
        int y_start = operator_size / 2.f + (3 - line.start) * space;
        int y_end = operator_size / 2.f + (3 - line.end) * space;
        path.moveTo(x_right, y_start);
        path.lineTo(width, y_start);
        path.lineTo(width, y_end);
        path.lineTo(x_right, y_end);
      } else {
        int y_start = operator_size + (3 - line.start) * space;
        int y_end = operator_size + (3 - line.end) * space;
        path.moveTo(x_middle, y_start);
        path.lineTo(x_middle, y_end);
        path.lineTo(x_middle, y_end);
        path.lineTo(x_middle, y_end);
      }
      paint.setColor(Colours::White);
      ctx.drawPath(path, paint);
    }

    // draw operators
    SkPaint fillPaint = OTTO_paint();
    SkPaint strokePaint = OTTO_paint();
    fillPaint.setStyle(SkPaint::kFill_Style);
    fillPaint.setColor(Colours::White);
    for (int i = 0; i < 4; i++) {
      float level = activity_levels[i];
      // Choose colour
      strokePaint.setColor(mix(operator_colours[i], Colours::White, 0.5*(i == cur_op)));

      if (algorithms[algorithm_idx].modulator_flags[i]) { // draw modulator
        SkRect rect = SkRect::MakeXYWH(x_middle - operator_radius * square_scale, (3 - i) * space, operator_size * square_scale, operator_size * square_scale);
        if (i == cur_op) ctx.drawRect(rect, fillPaint);
        ctx.drawRect(rect, strokePaint);
        // Draw activity level
        fillPaint.setColor(mix(operator_colours[i], Colours::White, 0.5*(i == cur_op)));
        rect.inset(operator_radius * square_scale * (1 - level), operator_radius * square_scale * (1 - level));
        ctx.drawRect(rect, fillPaint);

      } else { // draw carrier
        float y_pos = operator_radius + (3 - i) * space;
        if (i == cur_op) ctx.drawCircle(x_middle, y_pos, operator_radius, fillPaint);
        ctx.drawCircle(x_middle, y_pos, operator_radius, strokePaint);
        // Draw activity level
        fillPaint.setColor(mix(operator_colours[i], Colours::White, 0.5*(i == cur_op)));
        ctx.drawCircle(x_middle, y_pos, operator_radius * level, fillPaint);
      }
    }

    // draw arrowheads
    strokePaint.setStrokeWidth(4.f);
    strokePaint.setColor(Colours::White);
    strokePaint.setStyle(SkPaint::kStrokeAndFill_Style);
    int side_length = 5;
    SkPath path;
    for (auto&& line : algorithms[algorithm_idx].operator_lines) {
      if (line.side == Drawside::left) {
        int y = operator_radius + (3 - line.end) * space;
        path.moveTo(x_left, y);
        path.lineTo(x_left - side_length, y - side_length);
        path.lineTo(x_left - side_length, y + side_length);
        path.close();
      } else if (line.side == Drawside::right) {
        int y = operator_radius + (3 - line.end) * space;
        path.moveTo(x_right, y);
        path.lineTo(x_right + side_length, y - side_length);
        path.lineTo(x_right + side_length, y + side_length);
        path.close();
      } else {
        int y = (3 - line.end) * space;
        path.moveTo(x_middle, y);
        path.lineTo(x_middle + side_length, y - side_length);
        path.lineTo(x_middle - side_length, y - side_length);
        path.close();
      }
      ctx.drawPath(path, strokePaint);
    }
  }
};

void draw_envelopes(SkCanvas& ctx) {
  constexpr int active_y = 60;
  constexpr int not_active_y = 0;
  constexpr int y_pad = 40;
  constexpr int x_start = 90;
  constexpr int x_size = 190;
  float step = (ctx.imageInfo().height()  - 2 * y_pad - active_y - not_active_y * 3) / 3.f;

  // op_sizes are Choreograph outputs
  // By always moving them over the same duration, we make sure they always sum to zero.
  // For now:
  std::array<float, 4> op_sizes = {0.f, 0.f, 1.f, 0.f};
 
  int upper_y = y_pad;
  for (int i=0; i<4; i++) {
    int size_y = active_y * op_sizes[i] + not_active_y * (1 - op_sizes[i]);
    ADSR env = {.a = 0.3, .d = 0.3, .s = 0.3, .r = 0.3};
    env.bounding_box.move_to({x_start, upper_y});
    env.bounding_box.resize({x_size, size_y});
    env.draw(ctx);
    upper_y += size_y + step;
  } 
}

// Should be resized when expanding and compressing
struct FractionGraphic : otto::graphics::Widget<FractionGraphic> {
  int numerator, denominator;
  float expansion;
  bool active;
  FractionGraphic(int num, int denom, float exp, bool act) : numerator(num), denominator(denom), expansion(exp), active(act) {}

  void do_draw(SkCanvas& ctx) {
    SkPaint paint = OTTO_paint();
    if (active) paint.setColor(Colours::Blue);
    else paint.setColor(Colours::Grey50);

    const float width = bounding_box.width();
    const float height = bounding_box.height();
    
    // Text
    paint.setStrokeWidth(2.f);
    paint.setStyle(SkPaint::kStrokeAndFill_Style);
    sk_sp<SkTextBlob> n = SkTextBlob::MakeFromString(std::to_string(numerator).c_str(), SkFont(nullptr, 26));
    sk_sp<SkTextBlob> d = SkTextBlob::MakeFromString(std::to_string(denominator).c_str(), SkFont(nullptr, 26));
    //SkRect db = d.bounds();
    //paint.setTextAlign(SK_TextAlign::Left);
    SkRect rect = SkRect();
    SkFont font = SkFont(nullptr, 20);
    SkScalar wid = font.measureText("3", 1, SkTextEncoding(), &rect, &paint);
    ctx.drawTextBlob(n.get(), 0, rect.height(), paint);
    float denominator_y = std::max(height, rect.height());
    wid = font.measureText("2", 1, SkTextEncoding(), &rect, &paint);
    ctx.drawTextBlob(d.get(), width - rect.width(), denominator_y , paint);

    //  Line
    SkPath path;
    // It's hard to align text. Use this to adjust manually
    constexpr float fudge_factor = 1.5f;
    path.moveTo(0 + width * 0.5f * (1 - expansion) + fudge_factor, denominator_y);
    path.lineTo(0.5f * width  * (1 + expansion) + fudge_factor, 0);
    paint.setStrokeWidth(6.f);
    ctx.drawPath(path, paint);
  } 
};

struct DetuneGraphic : otto::graphics::Widget<DetuneGraphic> {
  float value;
  float expansion;
  bool active;
  DetuneGraphic(float v, float e, bool a) : value(v), expansion(e), active(a) {}

  void do_draw(SkCanvas& ctx) {
    SkPaint paint = OTTO_paint();
    if (active) paint.setColor(Colours::Blue);
    else paint.setColor(Colours::Grey50);
    paint.setStrokeWidth(2.f);
    paint.setStyle(SkPaint::kStrokeAndFill_Style);

    sk_sp<SkTextBlob> val = SkTextBlob::MakeFromString(std::to_string(value).c_str(), SkFont(nullptr, 26));
    sk_sp<SkTextBlob> dtune = SkTextBlob::MakeFromString("D.T", SkFont(nullptr, 26));
    ctx.drawTextBlob(val.get(), 0, 0, paint);
    //paint.setColor(mix(Colours::Black, paint.getColor(), expansion));
    ctx.drawTextBlob(dtune.get(), 0, 30, paint);
  }
};

struct LevelGraphic : otto::graphics::Widget<Levelgraphic> {
  
}

TEST_CASE ("Non-trivial graphics test") {
  auto app = start_app(ConfigManager::make(), Graphics::make());
  SUBCASE ("FM stub") {
    app.service<Graphics>().show([&](SkCanvas& ctx) {
     
      
      Operators ops(3, 2, {0.f, 0.f, 0.5f, 0.0f});
      ops.bounding_box.move_to({20, 30});
      ops.bounding_box.resize({55, 180});
      ops.draw(ctx);
      //draw_envelopes(ctx);

      FractionGraphic fract(3, 2, 1.f, true);
      fract.bounding_box.move_to({80, 20});
      fract.bounding_box.resize({40, 40});
      fract.draw(ctx);

      fract.bounding_box.move_to({150, 20});
      fract.bounding_box.resize({40, 32});
      fract.expansion = 0.5f;
      fract.draw(ctx);

      fract.bounding_box.move_to({220, 20});
      fract.bounding_box.resize({40, 12});
      fract.expansion = 0.0f;
      fract.active = false;
      fract.draw(ctx);

      DetuneGraphic detune(0.2f, 1.0f, true);
      detune.bounding_box.move_to({90, 100});
      detune.bounding_box.resize({50, 40});
      detune.draw(ctx);

      

    });
    std::this_thread::sleep_for(std::chrono::seconds(20));
    app.stop();
  }
}