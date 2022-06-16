#include "svg.h"

namespace svg {

    using namespace std::literals;

    // --- Object Properties ----------------------------------------------------------
    // ------ color -------------------------------------------------------------------
    std::ostream& operator<<(std::ostream& out, Color color)
    {
        std::ostringstream strm;
        visit(OstreamColor{ strm }, color);
        out << strm.str();
        return out;
    }

    // ------ other properties --------------------------------------------------------
    std::ostream& operator<<(std::ostream& out, const StrokeLineCap line)
    {
        if (line == StrokeLineCap::BUTT)
        {
            out << "butt";
        }
        else if (line == StrokeLineCap::ROUND)
        {
            out << "round";
        }
        else if (line == StrokeLineCap::SQUARE)
        {
            out << "square";
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin line)
    {
        if (line == StrokeLineJoin::ARCS)
        {
            out << "arcs";
        }
        else if (line == StrokeLineJoin::BEVEL)
        {
            out << "bevel";
        }
        else if (line == StrokeLineJoin::MITER)
        {
            out << "miter";
        }
        else if (line == StrokeLineJoin::MITER_CLIP)
        {
            out << "miter-clip";
        }
        else if (line == StrokeLineJoin::ROUND)
        {
            out << "round";
        }
        return out;
    }

    // --- RenderContext --------------------------------------------------------------
    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // --- Circle ---------------------------------------------------------------------
    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // --- Polyline -------------------------------------------------------------------
    Polyline& Polyline::AddPoint(Point new_point) {
        points_.push_back(new_point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool first_point = true;
        for (const auto& point : points_)
        {
            if (first_point)
            {
                out << point.x << ","sv << point.y;
                first_point = false;
            }
            else
            {
                out << " " << point.x << ","sv << point.y;
            }
        }
        out << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // --- Text -----------------------------------------------------------------------
    Text& Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        //<text x = "35" y = "20" dx = "0" dy = "6" font - size = "12" font - family = "Verdana" font - weight = "bold">Hello C++< / text>
        out << "<text"sv;
            RenderAttrs(context.out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv
            << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv
            << "font-size=\""sv << size_ << "\" "sv;
        if (!font_family_.empty())
        {
            out << "font-family=\""sv << font_family_ << "\" "sv;
        }
        if (!font_weight_.empty())
        {
            out << "font-weight=\""sv << font_weight_ << "\""sv;
        }
        out << ">"sv;
        for (const auto& c : data_)
        {
            if (c == '\"')
            {
                out << "&quot;"sv;
            }
            else if (c == '\'')
            {
                out << "&apos;"sv;
            }
            else if (c == '<')
            {
                out << "&lt;"sv;
            }
            else if (c == '>')
            {
                out << "&gt;"sv;
            }
            else if (c == '&')
            {
                out << "&amp;"sv;
            }
            else
            {
                out << c;
            }
        }
        out << "</text>"sv;
    }

    // --- Document -------------------------------------------------------------------
    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.push_back(move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        for (const auto& object : objects_)
        {
            (*object).Render(out);
        }

        out << "</svg>"sv << std::endl;
    }


}  // namespace svg