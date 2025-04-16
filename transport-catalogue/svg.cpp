#include "svg.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace svg
{

using namespace std::literals;

bool operator==(Point first, Point second)
{
    return (first.x - second.x < 1e-7) && (first.y - second.y < 1e-7);
}

bool operator!=(Point first, Point second)
{
    return !(first == second);
}

std::ostream& operator<<(std::ostream& out, const Point& elem)
{
    out << elem.x << "," << elem.y;
    return out;
}


void Object::Render(const RenderContext& context) const
{
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)
{
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)
{
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const
{
    auto& out = context.out;
    out << "  <circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    PathProps::RenderAttr(out);
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point)
{
    points_.push_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext &context) const
{
    auto& out = context.out;
    out << "  <polyline points=\""sv;
    bool first = true;
    for(const Point& point: points_)
    {
        if(first)
        {
            out << point.x << "," << point.y;
            first = false;
            continue;
        }
        out << " " << point.x << "," << point.y;
    }
    out << "\""sv;
    PathProps::RenderAttr(out);
    out << "/>"sv;
}


// ---------- Text ------------------

Text& Text::SetPosition(Point pos)
{
    start_point_ = pos;
    return *this;
}

Text& Text::SetFontSize(int32_t size)
{
    size_font_ = size;
    return *this;
}

Text& Text::SetOffset(Point offset)
{
    offset_ = offset;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family)
{
    name_font_ = std::move(font_family);
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight)
{
    font_weight_ = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string data)
{
    data_ = std::move(data);
    return *this;
}

void Text::HtmlEncodeString(std::ostream& out, const std::string_view sv) const
{
    for(char symbol : sv)
    {
        switch (symbol)
        {
            case '\'':
            {
                out << "&apos;"sv;
                break;
            }
            case '&':
            {
               out << "&amp;"sv;
               break;
            }
            case '"':
            {
                out << "&quot;"sv;
                break;
            }
            case '<':
            {
                out << "&lt;"sv;
                break;
            }
            case '>':
            {
                out << "&gt;"sv;
                break;
            }
    default:
        out.put(symbol);
        }
    }
}

void Text::RenderObject(const RenderContext &context) const
{
    auto& out = context.out;
    out << "  <text";
    PathProps::RenderAttr(out);
    out << " x=\"" << start_point_.x << "\" y=\"" << start_point_.y << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" " << "font-size=\"" << size_font_ << "\" ";

    if(!name_font_.empty())
    {
        out << "font-family=\"" << name_font_ << "\""; // Здесь вопрос может нужно вернуть пробел перед font
    }
    if(!font_weight_.empty())
    {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
    out << ">";
    HtmlEncodeString(out, data_);
    out << "</text>"sv;
}

// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object> &&obj)
{
    objects_.push_back(std::move(obj));
}

void Document::Render(std::ostream &out) const
{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for(const auto& element: objects_)
    {
        element->Render(out);
    }
    out << "</svg>"sv;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineCap& element)
{
    using namespace std::literals;
    switch (element)
    {
    case StrokeLineCap::BUTT:
    {
        out << "butt"sv;
        break;
    }
    case StrokeLineCap::ROUND:
    {
        out << "round"sv;
        break;
    }
    case StrokeLineCap::SQUARE:
    {
        out << "square"sv;
        break;
    }
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& element)
{
    using namespace std::literals;
    switch (element)
    {
    case StrokeLineJoin::ARCS :
      {
        out << "arcs"sv;
        break;
      }
    case StrokeLineJoin::BEVEL :
      {
        out << "bevel"sv;
        break;
      }
    case StrokeLineJoin::MITER :
      {
        out << "miter"sv;
        break;
      }
    case StrokeLineJoin::MITER_CLIP :
      {
        out << "miter-clip"sv;
        break;
      }
    case StrokeLineJoin::ROUND :
      {
        out << "round"sv;
        break;
      }
    }
    return out;
}

}


namespace shapes
{
Triangle::Triangle(svg::Point first, svg::Point second, svg::Point third)
    : p1_(first), p2_(second), p3_(third)
{}

void Triangle::Draw(svg::ObjectContainer &container) const
{
    container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
}

Star::Star(svg::Point center, double outer_radius, double inner_radius, int num_rays)
    : center_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays)
   {}

   svg::Polyline Star::CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) const
   {
       using namespace svg;
       Polyline polyline;
       for (int i = 0; i <= num_rays; ++i)
       {
           double angle = 2 * M_PI * (i % num_rays) / num_rays;
           polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
           if (i == num_rays)
           {
               break;
           }
           angle += M_PI / num_rays;
           polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
       }
       return polyline;
   }

   void Star::Draw(svg::ObjectContainer& container) const
   {
        container.Add(CreateStar(center_, outer_radius_, inner_radius_, num_rays_).SetFillColor("red").SetStrokeColor("black"));
   }

   Snowman::Snowman(svg::Point head_center, double head_radius)
       : head_center_(head_center), head_radius_(head_radius)
   {}

   void Snowman::Draw(svg::ObjectContainer &container) const
   {
       svg::Point third_circle = head_center_;
       third_circle.y += 5 * head_radius_;
       container.Add(svg::Circle().SetCenter(third_circle).SetRadius(2 * head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
       svg::Point second_circle = head_center_;
       second_circle.y += 2 * head_radius_;
       container.Add(svg::Circle().SetCenter(second_circle).SetRadius(1.5 * head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
       container.Add(svg::Circle().SetCenter(head_center_).SetRadius(head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black")); // first
   }
}
