#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <optional>


namespace svg
{
struct Rgb;
struct Rgba;
using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
//inline const Color NoneColor = static_cast<std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>>("none");

struct Rgb
{
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b)
        : red(r), green(g), blue(b)
    {}
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct Rgba
{
    Rgba()
        : red(0), green(0), blue(0), opacity(1)
    {}
    Rgba(uint8_t r, uint8_t g, uint8_t b, double alpha)
        : red(r), green(g), blue(b), opacity(alpha)
    {}
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    double opacity;
};

struct ColorPrinter
{
    std::ostream& out;

    void operator()(std::monostate) const
    {
        out << "none";
    }
    void operator()(std::string color) const
    {
        out << color;
    }
    void operator()(Rgb rgb) const
    {
        out <<"rgb(" <<  static_cast<uint16_t>(rgb.red) << "," << static_cast<uint16_t>(rgb.green) << "," << static_cast<uint16_t>(rgb.blue) << ")";
    }
    void operator()(Rgba rgba) const
    {
        out <<"rgba(" <<  static_cast<uint16_t>(rgba.red) << "," << static_cast<uint16_t>(rgba.green) << "," << static_cast<uint16_t>(rgba.blue) << "," << rgba.opacity << ")";
    }
};

enum class StrokeLineCap
{
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin
{
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

struct Point
{
    Point() = default;
    Point(double x, double y)
        : x(x),y(y)
    {}
    double x = 0;
    double y = 0;
};

bool operator==(Point first, Point second);
bool operator!=(Point first, Point second);
std::ostream& operator<<(std::ostream& output, const Point& element);
std::ostream& operator<<(std::ostream& output, const StrokeLineCap& element);
std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& element);
/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext
{
    RenderContext(std::ostream& out)
        : out(out)
    {}

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent)
    {}

    RenderContext Indented() const
    {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const
    {
        for(int i = 0; i < indent; ++i)
        {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

template<typename Owner>
class PathProps
{
public:
    PathProps() = default;

    Owner& SetFillColor(Color color)
    {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color)
    {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width)
    {
        stroke_width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap)
    {
        line_cap_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join)
    {
        line_join_ = line_join;
        return AsOwner();
    }

protected:

      void RenderAttr(std::ostream& out) const
      {
          using namespace std::literals;

          if (fill_color_.has_value())
          {
             out << " fill=\""sv;
             std::visit(ColorPrinter{out}, *fill_color_);
             out << "\""sv;
          }
          if (stroke_color_.has_value())
          {
              out << " stroke=\""sv;
              std::visit(ColorPrinter{out}, *stroke_color_);
              out << "\""sv;
          }
          if(stroke_width_.has_value())
          {
              out << " stroke-width=\""sv <<  *stroke_width_ << "\""sv;
          }
          if(line_cap_.has_value())
          {
             out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
          }
          if(line_join_.has_value())
          {
              out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
          }
      }

    ~PathProps() = default;

 private:

    Owner& AsOwner()
    {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};


/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object
{
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle>
{
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline>
{
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text>
{
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(int32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight); // Почему не число?? Чтобы по умолчанию не выводить?

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:
    void RenderObject(const RenderContext& context) const override; // Сделать вывод
    void HtmlEncodeString(std::ostream& out, const std::string_view sv) const;

    Point start_point_ = {};
    Point offset_ = {};
    uint32_t size_font_ = 1;
    std::string name_font_ = "";
    std::string font_weight_ = "";
    std::string data_ = "";
};

class ObjectContainer
{
public:
    template<typename T>
     void Add(T element_object);

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
protected:
     ~ObjectContainer() = default;
};

class Document : public ObjectContainer
{
public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;
private:
    std::vector<std::unique_ptr<Object>> objects_;
};

template<typename T>
void ObjectContainer::Add(T element_object)
{
    AddPtr(std::make_unique<T>(std::move(element_object)));
}

class Drawable
{
public:
    virtual void Draw(ObjectContainer& containe) const = 0;
    virtual ~Drawable() = default;
};

}  // namespace svg




namespace shapes
{
class Star : public svg::Drawable
{
public:
    Star(svg::Point center, double outer_radius, double inner_radius, int num_rays);
    void Draw(svg::ObjectContainer &container) const override;
private:
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) const;
    svg::Point center_;
    double outer_radius_;
    double inner_radius_;
    int num_rays_;
};

class Snowman : public svg::Drawable
{
public:
    Snowman(svg::Point head_center, double head_radius);
    void Draw(svg::ObjectContainer &containe) const override;
private:
    svg::Point head_center_;
    double head_radius_;
};

class Triangle : public svg::Drawable
{
public:
    Triangle(svg::Point first, svg::Point second, svg::Point third);
    void Draw(svg::ObjectContainer &container) const override;
private:
    svg::Point p1_;
    svg::Point p2_;
    svg::Point p3_;
};

}
